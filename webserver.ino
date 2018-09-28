#include <SPI.h>
#include <Ethernet.h>
#include <IRremote.h>

// Configurações de rede usada no Arduino
byte mac[] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(10, 0, 100, 20);
IPAddress myDns(8, 8, 8, 8);
IPAddress gateway(10, 0, 0, 2);
IPAddress subnet(255, 0, 0, 0);

// Ip interno do servidor boto
IPAddress nodeServer(10, 250, 250, 182);
//char nodeServer[] = "www.boto.museu-goeldi.br"; 

// Inicializando o cliente HTTP e o servidor na porta 80 
EthernetServer server(80);
EthernetClient client;

const int LM35 = A3; // Define o pino analógico que irá ler a saída do LM35
float temperature; // Variável que armazenará a temperatura medida

long lastReadingTime = 0;

IRsend irsend;

void setup()
{
  // Inicia comunicação serial, normalmente usada em desenvolvimento
  Serial.begin(9600);

  Ethernet.begin(mac, ip, myDns, gateway, subnet);
  server.begin();
  Serial.print("Servidor no ip: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  // Realiza uma checagem de temperatura a cada 60s 
  if (millis() - lastReadingTime > 60000) {
    readTemperature();
    postTemperature(temperature);
    Serial.println();
    lastReadingTime = millis();
  }

  listenForEthernetClients();
}

void listenForEthernetClients(){

  EthernetClient client = server.available();
  if (client) {
    Serial.println("Novo cliente conectado");
    // Requisição HTTP termina com uma linha branca
    boolean currentLineIsBlank = true;
    String request;

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;
        Serial.write(c);
        // Se chegamos ao fim da linha e a próxima linha está em branco
        // então a requisição HTTP foi finalizada.

        if (c == '\n' && currentLineIsBlank) {
          // Os seguintes blocos serão usados para controlar as centrais de ar.
          if (request.indexOf("/temperature-manager/api/ac/1?status=true") > 0){
            // send a standard http response header
            client.println(F("HTTP/1.1 200 OK"));
            client.println(F("Content-Type: text/html"));
            client.println(F("Connection: close")); // the connection will be closed after completion of the response        // refresh the page automatically every 5 sec
            client.println();
            client.println(F("<!DOCTYPE HTML>"));
            client.println(F("<html>"));
            client.println(F("<h1>ac1 = true</h1>"));
            client.println(F("</html>"));
            turnOnAc1();
          } else if (request.indexOf("/temperature-manager/api/ac/1?status=false") > 0) {
            // send a standard http response header
            client.println(F("HTTP/1.1 200 OK"));
            client.println(F("Content-Type: text/html"));
            client.println(F("Connection: close"); // the connection will be closed after completion of the response        // refresh the page automatically every 5 sec
            client.println();
            client.println(F("<!DOCTYPE HTML>"));
            client.println(F"<html>"));
            client.println(F("<h1>ac1 = false</h1>"));
            client.println(F("</html>"));
            turnOffAc1();
          } else if (request.indexOf("/temperature-manager/api/ac/2?status=true") > 0) {
            // send a standard http response header
            client.println(F("HTTP/1.1 200 OK"));
            client.println(F("Content-Type: text/html"));
            client.println(F("Connection: close")); // the connection will be closed after completion of the response        // refresh the page automatically every 5 sec
            client.println();
            client.println(F("<!DOCTYPE HTML>"));
            client.println(F("<html>"));
            client.println(F("<h1>ac2 = true</h1>"));
            client.println(F("</html>"));
            turnOnAc2();
          } else if (request.indexOf("/temperature-manager/api/ac/2?status=false") > 0) {
            // send a standard http response header
            client.println(F("HTTP/1.1 200 OK"));
            client.println(F("Content-Type: text/html"));
            client.println(F("Connection: close")); // the connection will be closed after completion of the response        // refresh the page automatically every 5 sec
            client.println();
            client.println(F("<!DOCTYPE HTML>"));
            client.println(F("<html>"));
            client.println(F("<h1>ac2 = false</h1>"));
            client.println(F("</html>"));
            turnOffAc2();
          }

          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    // Pequeno delay para o servidor receber as informações
    delay(10);
    // Fecha conexão
    client.stop();
    Serial.println(F("client disconnected"));
  }
}

void postTemperature(float temperature) {
  // Fecha qualquer conexão antes de enviar uma nova requisição
  // liberando assim o socket no shield de Ethernet
  client.stop();
  
  String temperatureStr = String(temperature);
  // Tenta realizar conexão
  if (client.connect(nodeServer, 3000)) {
    Serial.println(F("connected"));
    // Cria requisição GET
    client.println("GET /temperature-manager/api/lm?temperature=" + temperatureStr + " HTTP/1.1");
    client.println(F("Host: 10.250.250.182")); // Servidor node
    client.println(F("Connection: close"));
    client.println();

    Serial.println(F("temperature posted"));
  } else {
    Serial.println(F("connection failed"));
  }
}

// Checa temperatura
void readTemperature() {
  temperature = ( float(analogRead(LM35)) * 5 / (1023) ) / 0.01;
  Serial.print("Temperatura: ");
  Serial.println(temperature);
}

void turnOnAc1() {
    unsigned int vetor[198] = {6050,7400,500,600,550,550,550,600,500,600,550,600,500,550,550,600,550,500,550,600,550,550,550,550,550,600,550,550,550,550,500,600,550,600,500,600,500,600,550,550,550,550,500,600,550,550,550,550,550,600,550,600,500,550,550,550,600,550,550,600,550,550,550,600,500,600,550,600,550,550,550,550,550,550,550,600,500,550,600,550,550,550,550,550,550,600,550,550,550,600,550,550,550,550,550,550,550,600,550,550,550,550,550,550,600,550,500,600,500,600,500,600,550,550,550,600,550,1650,550,1650,550,550,500,600,550,550,550,600,500,600,550,600,550,550,500,600,550,550,550,600,500,600,550,550,600,550,550,1650,600,1600,550,600,500,600,550,550,550,1650,550,550,550,600,550,1650,500,550,600,1600,550,600,550,550,550,1650,550,550,550,1650,550,550,550,1650,500,1650,550,1600,500,600,550,550,550,1650,500,1700,500,7400,500,};
    irsend.sendRaw(vetor, 198, 38 /* kHz */);
}

void turnOffAc1() {
    unsigned int vetor[198] = {6100,7350,550,600,550,550,500,550,600,550,550,550,550,600,550,550,500,600,600,550,500,550,550,600,550,550,550,550,550,550,550,550,550,600,600,550,500,600,550,600,550,500,550,550,600,550,500,550,550,600,550,550,550,550,550,550,550,550,550,550,550,550,550,550,600,500,550,550,550,550,550,550,600,500,550,550,550,550,600,550,550,550,550,600,550,550,550,600,550,550,600,500,550,550,550,600,550,550,550,600,550,550,550,550,500,600,550,600,550,550,550,550,550,600,550,550,550,600,550,1600,550,550,500,650,550,550,550,550,550,600,550,550,550,550,500,600,600,550,550,550,500,600,600,500,550,600,550,1650,550,1650,550,550,550,550,550,550,600,1650,500,600,550,600,550,1650,550,500,550,1650,550,600,550,550,550,1650,550,550,550,1650,550,550,550,600,600,1600,500,1650,550,500,550,550,550,1650,550,1650,550,7350,550,};
    irsend.sendRaw(vetor, 198, 38);
}

void turnOnAc2() {
    unsigned int vetor[200] = {4300,4500,450,1700,450,650,450,1700,450,1700,450,650,450,600,450,1750,450,600,450,650,450,1700,450,650,450,600,450,1750,450,1700,450,600,450,1750,450,1700,450,650,450,1700,450,1750,450,1700,500,1700,450,1700,450,1700,450,650,450,1700,500,600,450,600,450,650,450,600,450,650,450,600,450,650,450,600,450,1750,450,1700,450,600,450,650,450,600,450,650,450,1700,450,1750,450,600,500,600,450,1700,450,1750,400,1750,450,1700,450,5300,4350,4450,450,1700,450,650,450,1700,450,1750,450,600,450,650,450,1700,500,600,450,600,450,1750,450,600,450,600,500,1700,450,1700,450,650,450,1700,450,1750,450,600,450,1750,450,1700,450,1750,450,1700,450,1700,450,1750,450,600,450,1750,450,600,450,650,450,600,450,650,450,600,450,650,450,600,450,650,450,1700,450,1700,450,650,450,600,450,650,450,600,450,1750,450,1700,450,650,450,600,450,1750,450,1700,450,1700,450,1750,450,};
    irsend.sendRaw(vetor, 200, 38 /* kHz */);
}

void turnOffAc2() {
    unsigned int vetor[200] = {4250,4500,450,1700,450,650,450,1750,400,1750,450,600,500,600,450,1700,450,650,400,650,450,1700,450,650,450,650,400,1750,450,1700,450,650,450,1750,400,650,450,1700,450,1700,450,1750,450,1750,400,650,450,1700,450,1750,450,1700,450,650,450,650,400,650,400,650,450,1750,400,650,450,600,450,1750,450,1750,400,1750,450,650,400,650,450,600,450,650,400,650,450,650,400,650,450,650,400,1750,450,1700,450,1750,450,1750,400,1750,450,5300,4300,4450,450,1750,450,650,400,1750,400,1750,450,650,400,650,450,1750,400,650,450,650,400,1750,450,650,400,650,450,1750,400,1750,400,650,450,1750,400,650,450,1750,400,1750,450,1750,400,1750,450,650,400,1750,450,1700,450,1750,400,650,450,650,400,650,450,600,450,1750,450,650,400,650,450,1700,450,1750,400,1750,450,650,400,650,450,650,400,650,450,650,400,650,450,650,400,650,450,1750,400,1750,450,1750,400,1750,400,1750,450,};
    irsend.sendRaw(vetor, 200, 38);
}