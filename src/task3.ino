#include <Arduino.h>
#include <ArduinoJson.h>

bool gameActive = false;
String playerChoices[3] = {"Rock", "Paper", "Scissors"};
int gameMode = 0;

struct GameConfig {
  int gameMode;
  String playerChoices[3];
};

void saveConfig(const GameConfig &config) {
  StaticJsonDocument<200> doc;
  doc["gameMode"] = config.gameMode;
  doc["playerChoices1"] = config.playerChoices[0];
  doc["playerChoices2"] = config.playerChoices[1];
  doc["playerChoices3"] = config.playerChoices[2];

  String output;
  serializeJson(doc, output);
  Serial.println(output);
}

bool loadStringConfig(JsonDocument& doc, const char* key, String& value) {
  if (doc.containsKey(key) && doc[key].is<String>()) {
    value = doc[key].as<String>();
    return true;
  }
  Serial.println(String(key) + " not found or invalid");
  return false;
}

void loadConfig(String jsonConfig) {
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, jsonConfig);

  if (error) {
    Serial.println("Failed to load configuration");
    return;
  }

  if (doc.containsKey("gameMode")) {
    gameMode = doc["gameMode"].as<int>();
  } else {
    Serial.println("gameMode not found");
    return;
  }

  if (!loadStringConfig(doc, "playerChoices1", playerChoices[0])) return;
  if (!loadStringConfig(doc, "playerChoices2", playerChoices[1])) return;
  if (!loadStringConfig(doc, "playerChoices3", playerChoices[2])) return;

  Serial.println("Configuration loaded!");
}

String generateAIChoice() {
  int randomIndex = random(0, 3);
  return playerChoices[randomIndex];
}

String determineWinner(String player1, String player2) {
  int indexRock = 0;
  int indexPaper = 1;
  int indexScissors = 2;

  if (player1 == player2) return "Draw";

  if ((player1 == playerChoices[indexRock] && player2 == playerChoices[indexScissors]) ||
      (player1 == playerChoices[indexPaper] && player2 == playerChoices[indexRock]) ||
      (player1 == playerChoices[indexScissors] && player2 == playerChoices[indexPaper])) {
    return "Player 1 wins!";
  }

  return "Player 2 wins!";
}

bool isValidMove(String move) {
  for (int i = 0; i < 3; i++) {
    if (move == playerChoices[i]) {
      return true;
    }
  }
  return false;
}

void initializeGame() {
  gameActive = true;
  Serial.println("Game started!");
}

void processHumanVsAI(String humanChoice) {
  if (!isValidMove(humanChoice)) {
    Serial.println("Invalid move. Valid moves are: Rock, Paper, Scissors.");
    return;
  }

  String aiChoice = generateAIChoice();
  Serial.println("AI chose: " + aiChoice);

  String result = determineWinner(humanChoice, aiChoice);
  Serial.println(result);
  gameActive = false;
}

void processAIvsAI() {
  String ai1Choice = generateAIChoice();
  String ai2Choice = generateAIChoice();

  Serial.println("AI 1 chose: " + ai1Choice);
  Serial.println("AI 2 chose: " + ai2Choice);

  String result = determineWinner(ai1Choice, ai2Choice);
  Serial.println(result);
  gameActive = false;
}

void processMove(String receivedMessage) {
  if (gameMode == 0) {
    processHumanVsAI(receivedMessage);
  } else if (gameMode == 2) {
    processAIvsAI();
  }
}

void handleGameMode(String receivedMessage) {
  if (receivedMessage == "modes 0") {
    gameMode = 0;
    Serial.println("Game mode: Human vs AI");
  } else if (receivedMessage == "modes 1") {
    gameMode = 1;
    Serial.println("Game mode: Human vs Human");
  } else if (receivedMessage == "modes 2") {
    gameMode = 2;
    Serial.println("Game mode: AI vs AI");
  }

  GameConfig config = {gameMode, {playerChoices[0], playerChoices[1], playerChoices[2]}};
  saveConfig(config);
}

void processReceivedMessage(String receivedMessage) {
  if (receivedMessage == "new") {
    initializeGame();
    if (gameMode == 2) {
      processAIvsAI();
    } else {
      Serial.println("Make your move (Rock, Paper, Scissors):");
    }
  } else if (receivedMessage.startsWith("modes")) {
    handleGameMode(receivedMessage);
  } else if (receivedMessage.startsWith("{")) {
    loadConfig(receivedMessage);
  } else if (gameActive) {
    processMove(receivedMessage);
  } else {
    Serial.println("No active game. Type 'new' to start.");
  }
}

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
}

void loop() {
  if (Serial.available() > 0) {
    String receivedMessage = Serial.readStringUntil('\n');
    receivedMessage.trim();
    processReceivedMessage(receivedMessage);
  }
}
