#include <Arduino.h>
#include <ArduinoJson.h>

bool gameActive = false;
String playerChoices[3] = {"Rock", "Paper", "Scissors"};
int gameMode = 0;

/**
 * @brief Represents the game configuration structure.
 * 
 * Contains the current game mode and player choices for the game.
 */
struct GameConfig {
  int gameMode;                 /**< Current game mode (0, 1, or 2). */
  String playerChoices[3];      /**< Array of player choices: Rock, Paper, Scissors. */
};

/**
 * @brief Saves the game configuration to JSON format.
 * 
 * @param config The game configuration to be saved.
 */
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

/**
 * @brief Loads a string configuration value from a JSON document.
 * 
 * @param doc The JSON document containing the configuration.
 * @param key The key for the value to load.
 * @param value The string variable to store the loaded value.
 * @return True if the key was found and valid; otherwise, false.
 */
bool loadStringConfig(JsonDocument& doc, const char* key, String& value) {
  if (doc.containsKey(key) && doc[key].is<String>()) {
    value = doc[key].as<String>();
    return true;
  }
  Serial.println(String(key) + " not found or invalid");
  return false;
}

/**
 * @brief Loads the game configuration from a JSON string.
 * 
 * @param jsonConfig The JSON string containing the configuration.
 */
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

/**
 * @brief Generates a random AI choice for the game.
 * 
 * @return A string representing the AI's choice (Rock, Paper, or Scissors).
 */
String generateAIChoice() {
  int randomIndex = random(0, 3);
  return playerChoices[randomIndex];
}

/**
 * @brief Determines the winner between two players.
 * 
 * @param player1 The choice of player 1.
 * @param player2 The choice of player 2.
 * @return A string indicating the winner or if it was a draw.
 */
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

/**
 * @brief Validates if a given move is valid.
 * 
 * @param move The move to validate.
 * @return True if the move is valid; otherwise, false.
 */
bool isValidMove(String move) {
  for (int i = 0; i < 3; i++) {
    if (move == playerChoices[i]) {
      return true;
    }
  }
  return false;
}

/**
 * @brief Initializes a new game.
 * 
 * Sets the game as active and prompts the player to start.
 */
void initializeGame() {
  gameActive = true;
  Serial.println("Game started!");
}

/**
 * @brief Processes a human vs AI game round.
 * 
 * @param humanChoice The human player's move.
 */
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

/**
 * @brief Simulates an AI vs AI game round.
 */
void processAIvsAI() {
  String ai1Choice = generateAIChoice();
  String ai2Choice = generateAIChoice();

  Serial.println("AI 1 chose: " + ai1Choice);
  Serial.println("AI 2 chose: " + ai2Choice);

  String result = determineWinner(ai1Choice, ai2Choice);
  Serial.println(result);
  gameActive = false;
}

/**
 * @brief Processes a move based on the game mode.
 * 
 * @param receivedMessage The received move or command.
 */
void processMove(String receivedMessage) {
  if (gameMode == 0) {
    processHumanVsAI(receivedMessage);
  } else if (gameMode == 2) {
    processAIvsAI();
  }
}

/**
 * @brief Handles changes to the game mode.
 * 
 * @param receivedMessage The message specifying the new game mode.
 */
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
}

/**
 * @brief Processes a received message and determines the appropriate action.
 * 
 * @param receivedMessage The received message or command.
 */
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
  } else if (receivedMessage.startsWith("save")) {
    GameConfig config = {gameMode, {playerChoices[0], playerChoices[1], playerChoices[2]}};
    saveConfig(config);
  } else if (receivedMessage.startsWith("{")) {
    loadConfig(receivedMessage);
  } else if (gameActive) {
    processMove(receivedMessage);
  } else {
    Serial.println("No active game. Type 'new' to start.");
  }
}

/**
 * @brief Initializes the serial communication and random seed.
 */
void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
}

/**
 * @brief Main loop to handle serial communication and game processing.
 */
void loop() {
  if (Serial.available() > 0) {
    String receivedMessage = Serial.readStringUntil('\n');
    receivedMessage.trim();
    processReceivedMessage(receivedMessage);
  }
}
