#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <NTPClient.h>

const char *wifi_ssid = "ssid";
const char *wifi_password = "password";

const char *auth_username = "username";
const char *auth_password = "password";

const char *device_name = "LightLamp";

const char *ntp_address = "0.hu.pool.ntp.org";
const int timezone_offset = 2 * 3600;

ESP8266WebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntp_address, timezone_offset);

//TIMES
int timeTargetMonday = 0;
int timeTargetTuesday = 0;
int timeTargetWednesday = 0;
int timeTargetThursday = 0;
int timeTargetFriday = 0;
int timeTargetSaturday = 0;
int timeTargetSunday = 0;

int loopTick = 0;

bool lampEnabled = false;
int lampBrightness = 75;

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
	switch (timeClient.getDay())
	{
	case 1:
		return timeTargetMonday;
	case 2:
		return timeTargetTuesday;
	case 3:
		return timeTargetWednesday;
	case 4:
		return timeTargetThursday;
	case 5:
		return timeTargetFriday;
	case 6:
		return timeTargetSaturday;
	case 0:
		return timeTargetSunday;
	default:
		Serial.println("ERROR!!! Day is not between 0-6");
		return 0;
	}
}

void handleRoot()
{
	const char *destructionMessage = R""""(Self-destruction started...)"""";
	server.send(202, "text/plain", destructionMessage);
}

void handle404()
{
	String message = "Path not found!\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += (server.method() == HTTP_GET) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";
	for (uint8_t i = 0; i < server.args(); i++)
	{
		message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
	}
	server.send(404, "text/plain", message);
}

void handleDebug()
{
	if (!server.authenticate(auth_username, auth_password))
	{
		return server.requestAuthentication();
	}

	String message = "<html>";
	message += "<head>";
	message += "<title>Debug?</title>";
	message += "</head>";

	message += "<body>";

	message += "<h1>Debug information of request</h1>";

	message += "<p>Time: ";
	message += timeClient.getFormattedTime();
	message += "</p>";

	message += "<p>URI: ";
	message += server.uri();
	message += "</p>";

	message += "<p>Method: ";
	message += (server.method() == HTTP_GET) ? "GET" : "POST";
	message += "</p>";

	message += "<p>Arguments: ";
	message += server.args();
	message += "</p>";
	message += "<ul>";
	for (uint8_t i = 0; i < server.args(); i++)
	{
		message += "<li> " + server.argName(i) + ": " + server.arg(i) + " </li>";
	}
	message += "</ul>";

	message += "<p>Headers: ";
	message += server.headers();
	message += "</p>";
	message += "<ul>";
	for (uint8_t i = 0; i < server.headers(); i++)
	{
		message += "<li> " + server.headerName(i) + ": " + server.header(i) + " </li>";
	}
	message += "</ul>";

	message += "</body>";
	message += "</html>";

	server.send(200, "text/html", message);
}

void handleStatus()
{
	if (!server.authenticate(auth_username, auth_password))
	{
		return server.requestAuthentication();
	}

	String message = "<!doctype html>";
	message += "<html lang=\"en\">";
	message += "<head>";
	message += "<title>Status</title>";
	message += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
	message += "<link href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T\" crossorigin=\"anonymous\">";
	message += "</head>";

	message += "<body>";

	message += "<h1>Lamp</h1>";
	message += "<form action=\"/toggle_lamp\">";
	message += "<input type=\"submit\" class=\"btn btn-warning\" style=\"display: block; margin: auto;\" value=\"";
	message += lampEnabled ? "Turn light off" : "Turn light on";
	message += "\"></form>";

	message += "<h1>Alarms</h1>";
	message += "<table class=\"table\">";
	message += "<thead>";
	message += "<tr>";
	message += "<th>Day</th>";
	message += "<th>Time</th>";
	message += "<th>Set new</th>";
	message += "</tr>";
	message += "</thead>";

	message += "<tbody>";
	message += "<tr>";
	//Day
	message += "<td>Monday</td>";
	//Time
	message += "<td>";
	message += timeTargetMonday / 3600;
	message += ":";
	message += (timeTargetMonday % 3600) / 60;
	message += "</td>";
	//Time picker
	message += "<td>";
	message += "<form action=\"/update_clock\">";
	message += "<input type=\"hidden\" name=\"day\" value=\"1\">";
	message += "<input type=\"time\" name=\"new_time\">";
	message += "<input type=\"submit\" class=\"btn btn-primary\"></form>";
	message += "</td>";
	message += "</tr>";

	message += "<tr>";
	//Day
	message += "<td>Tuesday</td>";
	//Time
	message += "<td>";
	message += timeTargetTuesday / 3600;
	message += ":";
	message += (timeTargetTuesday % 3600) / 60;
	message += "</td>";
	//Time picker
	message += "<td>";
	message += "<form action=\"/update_clock\">";
	message += "<input type=\"hidden\" name=\"day\" value=\"2\">";
	message += "<input type=\"time\" name=\"new_time\">";
	message += "<input type=\"submit\" class=\"btn btn-primary\"></form>";
	message += "</td>";
	message += "</tr>";

	message += "<tr>";
	//Day
	message += "<td>Wednesday</td>";
	//Time
	message += "<td>";
	message += timeTargetWednesday / 3600;
	message += ":";
	message += (timeTargetWednesday % 3600) / 60;
	message += "</td>";
	//Time picker
	message += "<td>";
	message += "<form action=\"/update_clock\">";
	message += "<input type=\"hidden\" name=\"day\" value=\"3\">";
	message += "<input type=\"time\" name=\"new_time\">";
	message += "<input type=\"submit\" class=\"btn btn-primary\"></form>";
	message += "</td>";
	message += "</tr>";

	message += "<tr>";
	//Day
	message += "<td>Thursday</td>";
	//Time
	message += "<td>";
	message += timeTargetThursday / 3600;
	message += ":";
	message += (timeTargetThursday % 3600) / 60;
	message += "</td>";
	//Time picker
	message += "<td>";
	message += "<form action=\"/update_clock\">";
	message += "<input type=\"hidden\" name=\"day\" value=\"4\">";
	message += "<input type=\"time\" name=\"new_time\">";
	message += "<input type=\"submit\" class=\"btn btn-primary\"></form>";
	message += "</td>";
	message += "</tr>";

	message += "<tr>";
	//Day
	message += "<td>Friday</td>";
	//Time
	message += "<td>";
	message += timeTargetFriday / 3600;
	message += ":";
	message += (timeTargetFriday % 3600) / 60;
	message += "</td>";
	//Time picker
	message += "<td>";
	message += "<form action=\"/update_clock\">";
	message += "<input type=\"hidden\" name=\"day\" value=\"5\">";
	message += "<input type=\"time\" name=\"new_time\">";
	message += "<input type=\"submit\" class=\"btn btn-primary\"></form>";
	message += "</td>";
	message += "</tr>";

	message += "<tr>";
	//Day
	message += "<td>Saturday</td>";
	//Time
	message += "<td>";
	message += timeTargetSaturday / 3600;
	message += ":";
	message += (timeTargetSaturday % 3600) / 60;
	message += "</td>";
	//Time picker
	message += "<td>";
	message += "<form action=\"/update_clock\">";
	message += "<input type=\"hidden\" name=\"day\" value=\"6\">";
	message += "<input type=\"time\" name=\"new_time\">";
	message += "<input type=\"submit\" class=\"btn btn-primary\"></form>";
	message += "</td>";
	message += "</tr>";

	message += "<tr>";
	//Day
	message += "<td>Sunday</td>";
	//Time
	message += "<td>";
	message += timeTargetSunday / 3600;
	message += ":";
	message += (timeTargetSunday % 3600) / 60;
	message += "</td>";
	//Time picker
	message += "<td>";
	message += "<form action=\"/update_clock\">";
	message += "<input type=\"hidden\" name=\"day\" value=\"7\">";
	message += "<input type=\"time\" name=\"new_time\">";
	message += "<input type=\"submit\" class=\"btn btn-primary\"></form>";
	message += "</td>";
	message += "</tr>";
	message += "</tbody>";

	message += "</table>";

	message += "<h1>Status</h1>";

	message += "<table class=\"table\">";
	message += "<thead>";
	message += "<tr>";
	message += "<th>Time</th>";
	message += "<th>Alarm at</th>";
	message += "<th>Time remaining</th>";
	message += "</tr>";
	message += "</thead>";

	message += "<tbody>";
	message += "<tr>";

	message += "<td>";
	message += timeClient.getFormattedTime();
	message += "</td>";
	message += "<td>";
	message += nextAlarm() / 3600;
	message += ":";
	message += (nextAlarm() % 3600) / 60;
	message += "</td>";
	message += "<td>";
	message += nextAlarm() - currentTimeMoment();
	message += "</td>";

	message += "</tr>";
	message += "</tbody>";
	message += "</table>";

	message += "</body>";
	message += "</html>";

	server.send(200, "text/html", message);
}

void handleUpdateTime()
{
	if (!server.authenticate(auth_username, auth_password))
	{
		return server.requestAuthentication();
	}

	int day = 0;
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

	if (day == 0)
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

	switch (day)
	{
	case 1:
		timeTargetMonday = newTarget;
		break;
	case 2:
		timeTargetTuesday = newTarget;
		break;
	case 3:
		timeTargetWednesday = newTarget;
		break;
	case 4:
		timeTargetThursday = newTarget;
		break;
	case 5:
		timeTargetFriday = newTarget;
		break;
	case 6:
		timeTargetSaturday = newTarget;
		break;
	case 7:
		timeTargetSunday = newTarget;
		break;
	}

	server.sendHeader("Location", "/status");
	server.send(301);
}

void handleToggleLamp(){
	lampEnabled = !lampEnabled;
	server.sendHeader("Location", "/status");
	server.send(301);
}

void setup(void)
{
	Serial.begin(9600);

	WiFi.mode(WIFI_STA);
	WiFi.hostname(device_name);
	WiFi.begin(wifi_ssid, wifi_password);

	Serial.println("");

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

	if (MDNS.begin("esp8266"))
	{
		Serial.println("MDNS responder started");
	}

	server.on("/", handleRoot);

	server.on("/inline", []() {
		server.send(200, "text/plain", "this works as well");
	});

	server.on("/debug", handleDebug);
	server.on("/update_clock", handleUpdateTime);
	server.on("/toggle_lamp", handleToggleLamp);
	server.on("/status", handleStatus);

	server.onNotFound(handle404);

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
	if(lampEnabled){
		changeLEDWithPercentage(lampBrightness);
		Serial.println("Lamp feature on!");
	}
	return lampEnabled;
}

void loop(void)
{
	server.handleClient();

	loopTick = (loopTick + 1) % 10;
	if (loopTick == 0)
	{
		if (!tickLamp())
			tickLight();
	}

	delay(100);
}