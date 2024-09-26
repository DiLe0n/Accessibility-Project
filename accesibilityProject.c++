#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pines para el botón de salto, joystick y buzzer
#define JUMP_BUTTON_PIN 26
#define DOWN_BUTTON_PIN 27
#define JOYSTICK_Y_PIN 34
#define BUZZER_PIN 14
#define MAX_Y 4095 // Valor máximo del eje Y del joystick

LiquidCrystal_I2C lcd(0x27, 16, 2); // Pantalla de 16x2

int colJugador = 0; // El jugador siempre estará en la columna izquierda (columna 0)
int rowJugador = 1; // Fila inicial del jugador (inicia en el suelo)

int colObstaculo = 15; // Columna inicial del obstáculo (lado derecho)
int rowObstaculo;      // Fila del obstáculo (puede variar aleatoriamente)

int score = 0; // Variable para la puntuación

// Definir el sprite del jugador
byte jugador[] = {
  B00100,
  B01010,
  B00100,
  B01110,
  B00100,
  B01110,
  B01010,
  B01010
};

// Definir el sprite del obstáculo
byte obstaculo[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, jugador);
  lcd.createChar(1, obstaculo); // Crear obstáculo

  pinMode(JUMP_BUTTON_PIN, INPUT_PULLUP); // Configurar el botón con pull-up
  pinMode(DOWN_BUTTON_PIN, INPUT_PULLUP); // Configurar el botón de bajada con pull-up
  pinMode(JOYSTICK_Y_PIN, INPUT);         // Configurar el pin del eje Y del joystick como entrada
  pinMode(BUZZER_PIN, OUTPUT);            // Configurar el pin del buzzer como salida

  lcd.setCursor(colJugador, rowJugador);
  lcd.write(0); // Dibujar al jugador en la posición inicial (en el suelo)

  // Inicializar la posición del obstáculo (fila aleatoria)
  rowObstaculo = random(0, 2);
}

void loop() {
  // Leer el estado del botón de salto y de bajada
  bool jumpButtonPressed = digitalRead(JUMP_BUTTON_PIN) == LOW;
  bool downButtonPressed = digitalRead(DOWN_BUTTON_PIN) == LOW;

  // Leer el valor del eje Y del joystick
  int joystickYValue = analogRead(JOYSTICK_Y_PIN);

  // Verificar el estado del jugador basado en la entrada de los botones
  if (jumpButtonPressed && rowJugador != 1) {
    rowJugador = 1; // Mover al jugador a la parte superior
    emitirPitidoAgudo(); // Emitir pitido agudo al subir
    actualizarPantalla(); // Actualizar la pantalla
  } 
  else if (downButtonPressed && rowJugador != 0) {
    rowJugador = 0; // Mover al jugador de vuelta al suelo
    emitirPitidoGrave(); // Emitir pitido grave al bajar
    actualizarPantalla(); // Actualizar la pantalla
  }

  // Controlar el movimiento del jugador con el joystick
  if (joystickYValue == MAX_Y && rowJugador != 1) {
    rowJugador = 1; // Mover al jugador de vuelta al suelo
    emitirPitidoGrave(); // Emitir pitido grave al bajar
    actualizarPantalla(); // Actualizar la pantalla
  } 
  else if (joystickYValue == 0 && rowJugador != 0) {
    rowJugador = 0; // Mover al jugador a la parte superior
    emitirPitidoAgudo(); // Emitir pitido agudo al subir
    actualizarPantalla(); // Actualizar la pantalla
  }

  // Mover el obstáculo de derecha a izquierda
  colObstaculo--;
  if (rowObstaculo == 1) {
    objetoProximoAbajo();
  } else {
    objetoProximoArriba();
  }

  // Si el obstáculo sale de la pantalla, generarlo de nuevo
  if (colObstaculo < 0) {
    score++;
    colObstaculo = 15;
    rowObstaculo = random(0, 2); // Generar fila aleatoria
  }

  if (score >= 10) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("You Win!"); // Mensaje de victoria
    emitirSonidoVictoria(); // Emitir sonido de victoria
    while (1); // Detener el juego
  }

  // Actualizar la pantalla con el obstáculo y el jugador
  actualizarPantalla();

  // Verificar colisión entre el jugador y el obstáculo
  if (colJugador == colObstaculo && rowJugador == rowObstaculo) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Game Over"); // Mensaje de fin de juego
    emitirSonidoGameOver(); // Emitir sonido de "Game Over"
    while (1); // Detener el juego
  }

  delay(200); // Retardo para controlar la velocidad del obstáculo
}


// Función para actualizar la pantalla LCD con el movimiento del jugador y el obstáculo
void actualizarPantalla() {
  lcd.clear(); // Limpiar la pantalla

  // Dibujar al jugador
  lcd.setCursor(colJugador, rowJugador);
  lcd.write(0);

  // Dibujar el obstáculo
  lcd.setCursor(colObstaculo, rowObstaculo);
  lcd.write(1); // Dibujar obstáculo
}

// Función para emitir un pitido agudo (frecuencia alta) al subir
void emitirPitidoAgudo() {
  tone(BUZZER_PIN, 1100, 100); // Emitir un tono de 1500 Hz durante 100 ms
}

// Función para emitir un pitido grave (frecuencia baja) al bajar
void emitirPitidoGrave() {
  tone(BUZZER_PIN, 700, 100); // Emitir un tono de 500 Hz durante 100 ms
}

void emitirSonidoGameOver() {
  tone(BUZZER_PIN, 100, 1000); // Emitir un tono de 100 Hz (muy grave) durante 1 segundo
}

void emitirSonidoVictoria() {
  tone(BUZZER_PIN, 2000, 1000); // Emitir un tono de 2000 Hz (muy agudo) durante 1 segundo
}

void objetoProximoAbajo(){
  tone(BUZZER_PIN, 500, 100);
}

void objetoProximoArriba(){
  tone(BUZZER_PIN, 1500, 100);
}
