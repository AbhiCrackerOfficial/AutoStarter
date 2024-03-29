#include <LittleFS.h>
#include <ArduinoJson.h>

String formatTime(NTPClient &timeInput) {
  int hour = timeInput.getHours();
  int minute = timeInput.getMinutes();
  String period = "AM";
  
  if (hour >= 12) {
    period = "PM";
    if (hour > 12) {
      hour -= 12;
    }
  }
  
  if (hour == 0) {
    hour = 12;
  }
  
  String hourStr = (hour < 10) ? "0" + String(hour) : String(hour);
  String minuteStr = (minute < 10) ? "0" + String(minute) : String(minute);
  
  return hourStr + ":" + minuteStr + " " + period;
}


const char* fname = "/data.txt"; // File to store the schedule

String load() {
  // Create a JSON object
  StaticJsonDocument<200> doc;
  doc["status"] = "false";
  JsonArray start_time_list = doc.createNestedArray("start_time_list");
  JsonArray end_time_list = doc.createNestedArray("end_time_list");
  // Convert the JSON object to a JSON string
  String jsonString;
  serializeJson(doc, jsonString);
  return jsonString;
}

void writeFile(String message)
{

  File file = LittleFS.open(fname, "w");
  if (!file)
  {
    Serial.println("DATA Failed to open file for writing");
    return;
  }

  if (file.print(message))
  {
    Serial.println("DATA File written");
  }
  else
  {
    Serial.println("DATA Write failed");
  }
  file.close();

}

String readFile() {
  File file = LittleFS.open(fname, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return "";
  }
  String data = file.readString();
  file.close();
  return data;
}

void updateStatus(const String &status) {
  String currentData = readFile();
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, currentData);
  if (error) {
    Serial.println("Failed to parse JSON data");
    return;
  }
  doc["status"] = status;
  String updatedData;
  serializeJson(doc, updatedData);
  writeFile(updatedData);
}

bool isDuplicate(const String& startTime, const String& endTime) {
  String currentData = readFile();

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, currentData);

  if (error) {
    Serial.println("Failed to parse JSON data");
    return false;
  }

  JsonArray startTimeList = doc["start_time_list"].as<JsonArray>();
  JsonArray endTimeList = doc["end_time_list"].as<JsonArray>();

  // Check for duplicacy by comparing the new start and end times with existing entries
  for (size_t i = 0; i < startTimeList.size(); i++) {
    if (startTimeList[i].as<String>() == startTime && endTimeList[i].as<String>() == endTime) {
      return true;
    }
  }

  return false;
}

void updateSchedule(const String& op, const String& startTime, const String& endTime) {
  if (op != "add" && op != "remove") {
    Serial.println("Invalid operation");
    return;
  }

  // Check for duplicacy when adding new entries
  if (op == "add" && isDuplicate(startTime, endTime)) {
    Serial.println("Duplicate entry");
    return;
  }

  String currentData = readFile();

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, currentData);

  if (error) {
    Serial.println("Failed to parse JSON data");
    return;
  }

  JsonArray startTimeList = doc["start_time_list"].as<JsonArray>();
  JsonArray endTimeList = doc["end_time_list"].as<JsonArray>();

  if (op == "add") {
    startTimeList.add(startTime);
    endTimeList.add(endTime);
  } else if (op == "remove") {
    // Find and remove matching start and end times from the arrays
    for (size_t i = 0; i < startTimeList.size(); i++) {
      if (startTimeList[i].as<String>() == startTime && endTimeList[i].as<String>() == endTime) {
        startTimeList.remove(i);
        endTimeList.remove(i);
        break;
      }
    }
  }

  String updatedData;
  serializeJson(doc, updatedData);

  writeFile(updatedData);
}

// Return values for checkSchedule function
enum class ScheduleAction {
  TurnOn,
  TurnOff,
  NoAction
};

// Check the schedule and determine whether to turn on or off the LED
ScheduleAction checkSchedule(JsonArray startTimeList, JsonArray endTimeList, String time) {
  for (size_t i = 0; i < startTimeList.size(); i++) {
    if (time == startTimeList[i].as<String>()) {
      return ScheduleAction::TurnOn; // Turn on LED
    } else if (time == endTimeList[i].as<String>()) {
      return ScheduleAction::TurnOff; // Turn off LED
    }
  }
  return ScheduleAction::NoAction; // No matching start or end time
}

void checkScheduleAndControlLED(String time) {
  String currentData = readFile();

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, currentData);

  if (error) {
    Serial.println("Failed to parse JSON data");
    return;
  }

  JsonArray startTimeList = doc["start_time_list"].as<JsonArray>();
  JsonArray endTimeList = doc["end_time_list"].as<JsonArray>();
  String status = doc["status"].as<String>();

  // Check if startTimeList and endTimeList are empty
  bool isScheduleEmpty = startTimeList.size() == 0 && endTimeList.size() == 0;

  if (isScheduleEmpty) {
    // No schedule defined, use the status directly
    if (status == "true") {
      digitalWrite(LED_BUILTIN, LOW); // Turn on LED
    } else {
      digitalWrite(LED_BUILTIN, HIGH); // Turn off LED
    }
    return;
  }

  // Determine the action based on the schedule
  ScheduleAction scheduleAction = checkSchedule(startTimeList, endTimeList, time);

  // Control the LED based on the schedule action
  switch (scheduleAction) {
    case ScheduleAction::TurnOn:
      digitalWrite(LED_BUILTIN, LOW); // Turn on LED
      break;
    case ScheduleAction::TurnOff:
      digitalWrite(LED_BUILTIN, HIGH); // Turn off LED
      break;
    case ScheduleAction::NoAction:
      // No matching start or end time, do nothing
      break;
  }
}
