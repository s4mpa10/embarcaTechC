/*
 * --------------------------------------------------------------------
 * CÓDIGO ADAPTADO PARA ESP32 / ESP32-S3
 * --------------------------------------------------------------------
 * Este código controla um leitor RFID MFRC522 para ler e gravar
 * nomes em tags MIFARE.
 *
 * Conexões para ESP32 (Barramento VSPI padrão):
 * MFRC522      ->  ESP32
 * ---------------------------
 * SDA / SS / CS  ->  GPIO 5  (Pino SS padrão)
 * SCK            ->  GPIO 18 (Pino SCK padrão)
 * MOSI           ->  GPIO 23 (Pino MOSI padrão)
 * MISO           ->  GPIO 19 (Pino MISO padrão)
 * RST            ->  GPIO 22 (Pino de Reset, pode ser outro)
 * 3.3V           ->  3.3V
 * GND            ->  GND
 * --------------------------------------------------------------------
 */

// As bibliotecas originais são compatíveis com o ESP32
#include <SPI.h>
#include <MFRC522.h>

// ========== ALTERAÇÃO PRINCIPAL: PINAGEM PARA O ESP32 ==========
#define SS_PIN    5  // (SDA/CS) Pino Slave Select padrão do SPI no ESP32
#define RST_PIN   22 // Pino de Reset. Pode ser qualquer GPIO livre.
#define LED_GREEN 4  // O pino 4 também está disponível no ESP32

// Cria uma instância do objeto MFRC522
MFRC522 rfid(SS_PIN, RST_PIN);

// Cria uma chave de acesso padrão para os cartões MIFARE (padrão de fábrica)
MFRC522::MIFARE_Key key;

// Define o bloco e o setor que usaremos para gravar e ler o nome
// ATENÇÃO: Não use o bloco 0, pois ele contém dados do fabricante e é somente leitura.
#define BLOCO_DADOS 4

// Protótipos de funções
void gravacaoDaTag();
void leituraDaTag();
void printHex(byte *buffer, byte bufferSize);
String getStatusCodeText(byte status);

void setup() {
  Serial.begin(9600); // ESP32 suporta velocidades mais altas, como 115200
  delay(1000);
  while (!Serial); // Aguarda a conexao serial

  SPI.begin();      // Inicia o barramento SPI (usará os pinos padrão do ESP32)
  rfid.PCD_Init();  // Inicia o modulo MFRC522
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, HIGH);

  // Preenche a chave com os valores padrão (0xFF)
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  
  Serial.println(F("Sistema RFID pronto para ESP32."));
  Serial.println(F("Aproxime uma tag para leitura."));
  Serial.println(F("Digite '1' e pressione Enter para entrar no modo de gravação."));
  Serial.println(F("-----------------------------------"));
}

void loop() {
  // --- VERIFICAÇÃO DE COMANDO DO USUÁRIO (NÃO-BLOQUEANTE) ---
  if (Serial.available() > 0) {
    char opcao = Serial.read();
    while(Serial.available()) { Serial.read(); } // Limpa o buffer de entrada

    if (opcao == '1') {
      gravacaoDaTag();
    }
  }

  // --- LEITURA CONSTANTE DE TAGS ---
  // Procura por novos cartões e tenta ler o serial
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    leituraDaTag();
    
    // Coloca a tag em modo de espera para permitir a leitura de outras tags
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
  
  delay(50); // Pequeno delay para estabilidade
}

// ==========================================================
// FUNÇÃO PARA EXIBIR INFORMAÇÕES DA TAG (LEITURA)
// ==========================================================
void leituraDaTag() {
  byte status;
  byte buffer[18];
  byte size = sizeof(buffer);

  // 1. Autentica o bloco para leitura
  status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, BLOCO_DADOS, &key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Falha na autenticação para leitura: "));
    Serial.println(getStatusCodeText(status));
    return;
  }

  // 2. Tenta ler os dados do bloco
  status = rfid.MIFARE_Read(BLOCO_DADOS, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Falha na leitura dos dados: "));
    Serial.println(getStatusCodeText(status));
    return;
  }

  // 3. Exibe as informações
  Serial.print(F("UID da Tag:"));
  printHex(rfid.uid.uidByte, rfid.uid.size);
  Serial.println();
  Serial.print(F("Funcionário: "));
  for (byte i = 0; i < 16; i++) { // O bloco MIFARE Classic tem 16 bytes de dados
    char c = (char)buffer[i];
    if (isprint(c)) {
      Serial.print(c);
    }
  }  
  Serial.println();
  Serial.println(F("Digite '1' e pressione Enter para entrar no modo de gravação."));
  Serial.println(F("-----------------------------------"));
}

// ==========================================================
// FUNCAO PARA CADASTRAR NOME NA TAG (GRAVAR DADOS)
// ==========================================================
void gravacaoDaTag() {
  byte status;
  byte buffer[16]; // Buffer para 16 bytes de dados (tamanho do bloco MIFARE)
  String entradaDigitada;

  Serial.println();
  Serial.println(F("--- MODO DE GRAVAÇÃO ---"));
  Serial.println(F("Digite o dado a ser gravado (max 16 caracteres) e pressione Enter."));
  Serial.println(F("Digite '0' para cancelar e sair."));

  // Aguarda o usuário digitar algo
  while (Serial.available() == 0) {
    delay(100);
  }

  entradaDigitada = Serial.readStringUntil('\n');
  entradaDigitada.trim();

  if (entradaDigitada == "0") {
    Serial.println(F("Modo de gravação cancelado."));
    Serial.println(F("-----------------------------------"));
    return;
  }

  if (entradaDigitada.length() > 0) {
    Serial.print(F("Dado a ser gravado: '"));
    Serial.print(entradaDigitada);
    Serial.println(F("'"));
    Serial.println(F("Aproxime o cartão para iniciar a gravação..."));
    
    // Implementação de timeout de 10 segundos
    unsigned long tempoInicial = millis();
    const unsigned long tempoLimite = 10000;

    // Aguarda uma tag ser apresentada
    while (!rfid.PICC_IsNewCardPresent()) {
      if (millis() - tempoInicial > tempoLimite) {
        Serial.println(F("Tempo esgotado! Nenhuma tag detectada."));
        Serial.println(F("Operação cancelada."));
        Serial.println(F("-----------------------------------"));
        return;
      }
      delay(50);
    }

    // Se a tag foi detectada, tenta ler seu serial
    if (!rfid.PICC_ReadCardSerial()) {
      Serial.println(F("Falha ao ler o serial da tag. Operação cancelada."));
      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();
      return;
    }

    Serial.print(F("Tag detectada! UID:"));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();

    // Prepara o buffer para gravação
    memset(buffer, ' ', sizeof(buffer)); // Preenche o buffer com espaços
    entradaDigitada.getBytes(buffer, sizeof(buffer) + 1); // Copia a string para o buffer

    // Autentica para escrita
    status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, BLOCO_DADOS, &key, &(rfid.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Falha na autenticação para escrita: "));
      Serial.println(getStatusCodeText(status));
      return;
    }

    // Grava os dados no bloco
//    status = rfid.MIFARE_Write(BLOCO_DADOS, buffer, 16);
//    if (status != MFRC522::STATUS_OK) {
//      Serial.print(F("Falha ao gravar na tag: "));
//      Serial.println(getStatusCodeText(status));
//    } else {
//      Serial.println(F("Dados gravados com sucesso!"));
//      Serial.println(F("-----------------------------------"));
//    }

    // --- Processo de gravacao ---
    Serial.println(F("Iniciando autenticacao para escrita..."));
    // Autentica o acesso ao setor do bloco (0x60 para Key A)
    status = rfid.PCD_Authenticate(0x60, BLOCO_DADOS, &key, &(rfid.uid));
    Serial.print(F("Status autenticacao (HEX): 0x"));
    Serial.println(status, HEX);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Falha na autenticacao para cadastro: "));
      Serial.println(getStatusCodeText(status));
      rfid.PICC_HaltA();       // Coloca a tag em modo de espera
      rfid.PCD_StopCrypto1(); // Encerra a criptografia
      return;
    }
    Serial.println(F("Autenticacao OK."));
    Serial.println(F("-----------------------------------"));
  
    Serial.println(F("Iniciando gravacao do bloco..."));
    // Grava os dados no bloco
    status = rfid.MIFARE_Write(BLOCO_DADOS, buffer, 16);
    Serial.print(F("Status gravacao (HEX): 0x"));
    Serial.println(status, HEX);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Falha no cadastro da tag: "));
      Serial.println(getStatusCodeText(status));
    } else {
      Serial.println(F("Cadastro realizado com sucesso!"));
      Serial.print(F("UID do Funcionario: "));
      printHex(rfid.uid.uidByte, rfid.uid.size);
      Serial.println();
      Serial.print(F("Nome Cadastrado: "));
      Serial.println(entradaDigitada); // Exibe o nome que acabou de ser gravado
      Serial.println(F("-----------------------------------"));
    }  

    // Finaliza a comunicação
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();

  } else {
    Serial.println(F("Nenhum dado digitado. Operação cancelada."));
  }
}

// ==========================================================
// FUNCOES AUXILIARES (sem alterações)
// ==========================================================

// Imprime um array de bytes em formato hexadecimal
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

// Retorna uma string descritiva para um código de status do MFRC522
String getStatusCodeText(byte status) {
  switch (status) {
    case MFRC522::STATUS_OK: return F("OK");
    case MFRC522::STATUS_ERROR: return F("Erro de comunicação");
    case MFRC522::STATUS_COLLISION: return F("Colisão de tag detectada");
    case MFRC522::STATUS_TIMEOUT: return F("Timeout na comunicação");
    case MFRC522::STATUS_MIFARE_NACK: return F("NACK recebido da tag");
    default: return F("Erro desconhecido");
  }
}
