#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// Wifi network station credentials
#define WIFI_SSID "The WiFi"
#define WIFI_PASSWORD "madhus2022"

#define BOT_TOKEN "5041615120:AAEtwwT9j_LmITNVNd8ug-KVXbkUfNUoY1Q"

const unsigned long BOT_MTBS = 1000; // mean time between scan messages

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; // last time messages' scan has been done

const int lightPin = LED_BUILTIN;
int lightStatus = 0;

void handleNewMessages(int numNewMessages)
{
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    if (text == "/lighton")
    {
      digitalWrite(lightPin, LOW); // turn the light on (HIGH is the voltage level)
      lightStatus = 1;
      bot.sendMessage(chat_id, "Light is ON", "");
    }

    if (text == "/lightoff")
    {
      lightStatus = 0;
      digitalWrite(lightPin, HIGH); // turn the light off (LOW is the voltage level)
      bot.sendMessage(chat_id, "Light is OFF", "");
    }

    if (text == "/status")
    {
      if (lightStatus)
      {
        bot.sendMessage(chat_id, "Light is ON", "");
      }
      else
      {
        bot.sendMessage(chat_id, "Light is OFF", "");
      }
    }

    if (text == "/start")
    {
      String welcome = "Telegram Based Home Automation, " + from_name + ".\n";
      welcome += "This is Telegram Bot example.\n\n";
      welcome += "/lighton : to switch the light ON\n";
      welcome += "/lightoff : to switch the light OFF\n";
      welcome += "/status : Returns current status of light\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}


void setup()
{
  Serial.begin(115200);
  Serial.println();

  pinMode(lightPin, OUTPUT); // initialize digital lightPin as an output.
  delay(10);
  digitalWrite(lightPin, HIGH); // initialize pin as off (active LOW)

  // attempt to connect to Wifi network:
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);
}

void loop()
{
  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}
