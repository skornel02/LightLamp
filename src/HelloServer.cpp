#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <NTPClient.h>

const char *wifi_ssid = "ssid";
const char *wifi_password = "pass";

const char *auth_username = "username";
const char *auth_password = "userpass";

const char *device_name = "LightLamp";

const char *ntp_address = "europe.pool.ntp.org";
const int timezone_offset = 1;

const IPAddress local_IP(192, 168, 0, 110);
const IPAddress gateway(192, 168, 0, 1);
const IPAddress subnet(255, 255, 255, 0);
const IPAddress dns1(8, 8, 8, 8);

ESP8266WebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntp_address, timezone_offset * 3600);

//TIMESv2
int timeTargets[7];

int loopTick = 0;

bool lampEnabled = false;
int lampBrightness = 75;

int wifiRestarter = 0;

int currentTimeMoment()
{
	timeClient.update();
	int hour = timeClient.getHours() * 3600;
	int minute = timeClient.getMinutes() * 60;
	int sec = timeClient.getSeconds();

	return hour + minute + sec;
}

void changeLEDWithPercentage(int percentage)
{
	if (percentage < 0 || percentage > 100)
		return;

	int realLevel = (((double)percentage / 100) * 255);
	analogWrite(4, realLevel);
	Serial.print("Setting LED to percentage: ");
	Serial.print(percentage);
	Serial.print("% digital: ");
	Serial.println(realLevel);
}

int nextAlarm()
{
	timeClient.update();
	return timeTargets[timeClient.getDay()];
}

String formatTime(int time)
{
	String timeStr = "";
	if(time != 0)
	{
	int hour = time / 3600;
	int minute = (time % 3600) / 60;
	timeStr += hour < 10 ? "0" : "";
	timeStr += hour;
	timeStr += ":";
	timeStr += minute < 10 ? "0" : "";
	timeStr += minute;
	}
	else 
	{
		timeStr = "--:--";
	}
	
	return timeStr;
}

String formatDay(int day)
{
	switch (day)
	{
		case 0:
			return "Sunday";
		case 1:
			return "Monday";
		case 2:
			return "Tuesday";
		case 3:
			return "Wednesday";
		case 4:
			return "Thursday";
		case 5:
			return "Friday";
		case 6:
			return "Saturday";
		default:
			return "Unrecognized day";
	}
}

void handleStatus()
{
	if (!server.authenticate(auth_username, auth_password))
	{
		return server.requestAuthentication();
	}

	String html = R"V0G0N(
<!doctype html>
<head>
	<title>Status</title>
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<link href="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T" crossorigin="anonymous">
	</head>
	<body>
		<h1>Lamp</h1>
		<form action="/toggle_lamp">
			<input type="submit" class="btn btn-warning" style="display: block; margin: auto;" value=")V0G0N";
	html += (lampEnabled ? "Turn light off" : "Turn light on");
	html += R"V0G0N(">
		</form>

		<h1>Alarms</h1>
		<table class="table">
			<thead>
				<tr>
					<th>Day</th>
					<th>Time</th>
					<th>Set new</th>
					<th>Reset</th>
				</tr>
			</thead>
			<tbody>)V0G0N";

	for(int day = 0 ; day < 7 ; day++)
	{
		int timeTarget = timeTargets[day];
		html += R"V0G0N(
				<tr>
					<td>)V0G0N";
		html += formatDay(day);
		html += R"V0G0N(</td>
					<td>)V0G0N";
	
		html += formatTime(timeTarget);

		html += R"V0G0N(</td>
					<td>
						<form action="/update_clock">
							<input type="hidden" name="day" value=")V0G0N";
		html += day;
		html += R"V0G0N(">
							<input type="time" name="new_time">
							<input type="submit" class="btn btn-primary" value="Update">
						</form>
					</td>
					<td>
						<form action="/remove_clock">
							<input type="hidden" name="day" value=")V0G0N";
		html += day;
		html += R"V0G0N(">
							<input type="submit" class="btn btn-danger" value="Reset">
						</form>
					</td>
				</tr>)V0G0N";
	}
		
	html += R"V0G0N(
				</tbody>
		</table>
		<h1>Status</h1>
		<table class="table">
			<thead>
				<tr>
					<th>Time</th>
					<th>Alarm at</th>
					<th>Time remaining</th>
				</tr>
			</thead>
			<tbody>
				<tr>
					<td>
						)V0G0N";
	html += timeClient.getFormattedTime();
	html += R"V0G0N(
					</td>
					<td>
						)V0G0N";
	html += formatTime(nextAlarm());
	html += R"V0G0N(
					</td>
					<td>
						)V0G0N";
	html += nextAlarm() - currentTimeMoment();
	html += R"V0G0N(
					</td>
				</tr>
			</tbody>
		</table>
	</body>
</html>)V0G0N";
	server.send(200, "text/html", html);
}

void handleUpdateTime()
{
	if (!server.authenticate(auth_username, auth_password))
	{
		return server.requestAuthentication();
	}

	int day = -1;
	String timeUnrefined;
	int hour = 0;
	int minute = 0;

	for (uint8_t i = 0; i < server.args(); i++)
	{
		if (server.argName(i).equalsIgnoreCase("day"))
		{
			String inputDay = server.arg(i);
			day = inputDay.toInt();
		}
		if (server.argName(i).equalsIgnoreCase("new_time"))
		{
			String inputTime = server.arg(i);
			timeUnrefined = inputTime;
		}
	}

	if (day == -1)
	{
		return server.send(400, "text/plain", "Day can't be null!");
	}
	if (timeUnrefined.length() != 5)
	{
		return server.send(400, "text/plain", "Time must be 5 character long and comply to 00:00 format!");
	}

	String hourSub = timeUnrefined.substring(0, 2);
	hour = hourSub.toInt();
	String minuteSub = timeUnrefined.substring(3, 5);
	minute = minuteSub.toInt();

	int newTarget = hour * 3600 + minute * 60;

	timeTargets[day] = newTarget;

	server.sendHeader("Location", "/status");
	server.sendHeader("Cache-Control", "max-age=1");
	server.send(301);
}

void handleRemoveTime()
{
	if (!server.authenticate(auth_username, auth_password))
	{
		return server.requestAuthentication();
	}

	int day = -1;

	for (uint8_t i = 0; i < server.args(); i++)
	{
		if (server.argName(i).equalsIgnoreCase("day"))
		{
			String inputDay = server.arg(i);
			day = inputDay.toInt();
		}
	}

	if (day == -1)
	{
		return server.send(400, "text/plain", "Day can't be null!");
	}

	timeTargets[day] = 0;

	server.sendHeader("Location", "/status");
	server.sendHeader("Cache-Control", "max-age=1");
	server.send(301);
}

void handleToggleLamp()
{
	Serial.println("Swapping lamp status");
	lampEnabled = !lampEnabled;
	server.sendHeader("Location", "/status");
	server.sendHeader("Cache-Control", "max-age=1");
	server.send(301);
}

void startWifi()
{
	Serial.println("Enabling wifi...");
	WiFi.begin(wifi_ssid, wifi_password);

	if (!WiFi.config(local_IP, gateway, subnet, dns1))
	{
		Serial.println("STA Failed to configure");
	}

	WiFi.mode(WIFI_STA);
	WiFi.hostname(device_name);

	// Wait for connection
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.print("Connected to ");
	Serial.println(wifi_ssid);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

	if (MDNS.begin("lightlamp"))
	{
		Serial.println("MDNS responder started");
	}
}

void stopWifi()
{
	Serial.println("Disabling wifi...");
	WiFi.mode(WIFI_OFF);
}

void setup(void)
{
	for(int i = 0 ; i < 7 ; i++)
	{
		timeTargets[i] = 0;
	}

	Serial.begin(9600);
	Serial.println("");

	startWifi();

	server.on("/update_clock", handleUpdateTime);
	server.on("/remove_clock", handleRemoveTime);
	server.on("/toggle_lamp", handleToggleLamp);
	server.on("/status", handleStatus);

	server.begin();
	Serial.println("HTTP server started");

	timeClient.begin();
	Serial.println("Time client started");

	timeClient.update();
	Serial.print("Current time is: day ");
	Serial.print(timeClient.getDay());
	Serial.print(" ");
	Serial.print(timeClient.getHours());
	Serial.print(":");
	Serial.print(timeClient.getMinutes());
	Serial.print(":");
	Serial.print(timeClient.getSeconds());
	Serial.print(" moment: ");
	Serial.println(currentTimeMoment());
}

void tickLight()
{
	int alarm = nextAlarm();
	if (alarm == 0)
	{
		Serial.println("No alarm for today!");
		changeLEDWithPercentage(0);
	}
	else
	{
		int difference = alarm - currentTimeMoment();
		if (difference >= 0 && difference < 1800)
		{
			//maximum: 85%
			//minimum: 0.1%
			int value = 85 - (difference / 21.2);
			changeLEDWithPercentage(value);

			Serial.print("Waking up in ");
			Serial.print(difference);
			Serial.print(" ... dim value: ");
			Serial.println(value);
		}
		else if (difference >= -300 && difference < 0)
		{
			//Max: 84.3%
			//Min: -0.7% techinally, but in reallity, 0.2%
			int value = 85 + (difference / 3.5);
			changeLEDWithPercentage(value);
			Serial.print("You have will have been already waken up in ");
			Serial.print(difference);
			Serial.print("... dim: ");
			Serial.println(value);
		}
		else
		{
			Serial.print("Nothing in range (off by:");
			Serial.print(difference > 0 ? difference - 1800 : difference);
			Serial.println(")... idling");

			changeLEDWithPercentage(0);
		}
	}
}

bool tickLamp()
{
	if (lampEnabled)
	{
		changeLEDWithPercentage(lampBrightness);
		Serial.println("Lamp feature on!");
	}
	return lampEnabled;
}

void loop(void)
{
	wifiRestarter++;

	server.handleClient();

	loopTick = (loopTick + 1) % 100;
	if (loopTick == 0)
	{
		timeClient.update();
		if (!tickLamp())
			tickLight();
	}

	if (wifiRestarter > 8640000)
	{
		wifiRestarter = 0;
		stopWifi();
		delay(500);
		startWifi();
	}

	delay(10);
}