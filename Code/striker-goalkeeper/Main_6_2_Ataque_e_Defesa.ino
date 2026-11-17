#include <string.h> // Biblioteca para manipulacao de strings
#include <math.h> // Biblioteca para funcoes e procedimentos matematicos

// +---------------------------------------------------------------------+
// |                      MODO DE JOGO                                   |
// +---------------------------------------------------------------------+
#define MODO 1 //ataque: 1, defesa: 0
#define GOAL 1 //1: amarelo, 2: azul

#define TX2 4 
#define RX2 5
#define MAX_PWM 4000 // Valor maximo do pwm --> cuidado para nao bater o fundo da escala, usar MAX_PWM - 1
#define INT_WINDUP 3300 // Valor limite para a integral
#define ENC1 17 // Porta do encoder 1 ***
#define ENC2 15 // Porta do encoder 2 ***
#define ENC3 16 // Porta do encoder 3 ***
#define ENC4 18 // Porta do encoder 4 ***
#define PPR 490.0 // Pulsos/Ticks por revolucao 
// Taxas de amostragem:
#define TS_us 20000 // Tempo de amostragem (us)
#define TS_US2 30000 // Tempo de amostragem (us) 2º loop
#define TS_US_FOT 4000 // Tempo de amostragem (us) ler linha. Ainda ha possibilidade de diminuir para ate 2000
//---------------------------------------------------------------------------

// kicker
#define TIME_FORWARD 400
#define TIME_BACKWARD 500
//---------------------------------------------------------------------------

#define ALPHA 0.7 // Confiabilidade na medição atual ou anterior

// Malhas de controle:

// Malha de controle do motor
#define KP 14.0
#define KI 6.0

// Malha de controle da BNO:
#define KP_BUS 0.1 //180 * KP = w max (0.08 OK)

// Malha de controle da camera
#define KP_CAM 0.05

//Malha de controle da bola
#define KP_BALL 7

// Malha de controle do defensor:
#define KP_def 42.0 //Anteriormente 15.0
//---------------------------------------------------------------------------

#define R 34 // Raio da roda
#define L 105 // Distância entre o centro e as rodas
//Mux control pins certo
#define s0 19
#define s1 20
#define s2 21
#define s3 22
//Mux SIG ccerto
#define SIG_pin 28

#define TAM_MSG 48

//Comunicacao:
char msgCom[TAM_MSG]; // Mensagem (string)
uint8_t posCom = 0; // Posicao na mensagem
bool descartarCom = true;   // só começa a gravar após o primeiro '$'
uint32_t okCount = 0, errCount = 0; // Contadores de mensagem
//------------------------------------------------------------------------

// Variaveis dos dados da camera:
int yState;
int yAngle;
int ySize;
int bState;
int bAngle;
int bSize;
int oState;
int oAngle;
int oDy;
//------------------------------------------------------------------------

// LISTAS
//------------------------------------------------------------------------
// Motores:
const int MA_pins[4] = {8, 6, 7, 9}; // Pinos digitais da ponte H
const int PWM_pins[4] = {2, 0, 1, 3}; // Pinos PWM
float ref_rpm[4] = {0.0}; // Velocidade referência em rpm
volatile uint32_t pulsos[4] = {0}; // Pulsos
float v_rpm[4] = {0.0}; // Velocidade angular atual em RPM
float v_rpm_0[4] = {0.0}; // Velocidade angular anterior em RPM
float vf_rpm[4] = {0.0}; // Velocidade final
float error[4] = {0.0}; // Erro atual
float error_0[4] = {0.0}; //Erro anterior
float integral[4] = {0.0}; //Integral atual
float integral_0[4] = {0.0}; //Integral anterior
float proportional[4] = {0.0}; // Proporcional atual
float proportional_0[4] = {0.0}; //Proporcional anterior
int total_pi[4] = {0}; // Valor final calculado pelo PI
uint32_t delta_pulsos[4] = {0}; // Quantidade de pulsos dentro do intervalo de amostragem


//------------------------------------------------------------------------

float ang_distort = 0.0;
uint32_t flag_kick = 0;


//Fototransistor
bool fototransistores_leitura[16] = {false};
bool state_cs = 0;
const double senList[16] = {0.000000, 0.382683, 0.707107, 0.923880, 1.000000, 0.923880, 0.707107, 0.382683, 0.000000, -0.382683, -0.707107, -0.923880, -1.000000, -0.923880, -0.707107, -0.382683};
const double cosList[16] = {1.000000, 0.923880, 0.707107, 0.382683, 0.000000, -0.382683, -0.707107, -0.923880, -1.000000, -0.923880,-0.707107, -0.382683, 0.000000, 0.382683, 0.707107,0.923880};
//Começa no 0º/360º

//------------------------------------------------------------------------
// LISTAS
//------------------------------------------------------------------------
const uint32_t N = 7e6/TS_us; // Numero de amostras
float log_w_rpm[N]; // Vetor para o log de amostras
volatile int pos = 0;
int media_bus = 0;
//Kicker
const int portaKicker = 12;
bool isKicking = false;
bool isDefesa = false;
uint32_t flag_manobra;
static uint32_t lastKicking;
int minKickingTime =  1000;
const int t_kicker = 30; //milissegundos anteriormente 50

//Comunicação
int tamanho = 3;
int BallAng = 0;
int BallDist = 1500;
int RobotAng = 0;
int bnoAngAtual = 0;
float pd_bus = 0.0;
// Kicker:
int cntKick;
int cnt_dist = 0;
int sinal_ref;
//------------------------------------------------------------------------

bool zona_frente = false;
bool zona_tras = false;
bool zona_esq = false;
bool zona_dir = false;
bool saindo_linha = false;
bool set1 = false, set2 = false;
//------------------------------------------------------------------------
// GABARITO
//------------------------------------------------------------------------
// Contagem dos pulsos
void ISR_ENC1();
void ISR_ENC2();
void ISR_ENC3();
void ISR_ENC4();
void Imprimir_pulsos();
void Imprimir_Velocidades();
void velocidade(float, float, float); // Movimento desejado (vx, vy, w) em comandos individuais de velocidade em RPM
void print_rpm(int); // Procedimento para imprimir a velocidade (RPM)
void controle_pwm(int []);
void Desloca (float v, float teta, float w);
// BNO:
int Wrap180( int ); // Funcao para manter um angulo entre [-180, 180)
int Wrap360( int ); // Funcao para manter um angulo entre 0 e 359
int MediaCircular (int now_ang); // Funcao para implentar uma media nos valores do BNO
float PD_bno(int OFFSET, int bno_ang); // Funcao para calcular a correcao para a orientacao do robo com base no BNO
//------------------------------------------------------------------------
// kicker:
void kick ();
void AtualizarKicker();
void InterrompeKicker();
//Fototransistor
bool DetectarLinha();
int readMux(int channel);
void SairLinhaAtaque(float);
void ImprimirMUX();
float AnguloFot();
void DetectarLinha2();
//Comunicação
void LeituraCom();
void ProcessarMensagem();
void ImprimirCom();
//Ataque
float AngleDistortion();
void Defender();
// Camera:
float PD_Cam(int CamAngle);
//Bola
float corrBall();
//------------------------------------------------------------------------
// SETUP MOTORES
//------------------------------------------------------------------------

void setup() 
{
  Serial.begin(115200); // Inicia a comunicacao Serial a 115200 bauds
  delay(5);
  //Serial.println("Aqui");
  //while(1)Serial.println("DEBUG");
  pinMode(25, OUTPUT);
  digitalWrite(25, HIGH);
  Serial2.setTX(TX2);
  Serial2.setRX(RX2);
  Serial2.begin(115200);
  delay(5);
  Serial.println("Iniciando...");
  // Encoder como entrada, liga resistor de pull up interno
  pinMode(ENC1, INPUT_PULLUP);
  pinMode(ENC2, INPUT_PULLUP);
  pinMode(ENC3, INPUT_PULLUP);
  pinMode(ENC4, INPUT_PULLUP);

  // Ativa a interrupcao por borda de subida
  attachInterrupt(digitalPinToInterrupt(ENC1), ISR_ENC1, CHANGE); 
  attachInterrupt(digitalPinToInterrupt(ENC2), ISR_ENC2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC3), ISR_ENC3, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC4), ISR_ENC4, CHANGE);

  delay(5);
  
  for (int i=0; i<4; i++){
    pinMode(MA_pins[i], OUTPUT);
    pinMode(PWM_pins[i], OUTPUT);
  }

  analogWriteFreq(20000);
  analogWriteRange(MAX_PWM);
  delay(10);
  //22
  set1 = true;
  while( !set1 || !set2)Serial.println("Aguardando");
}

//------------------------------------------------------------------------
// LOOP MOTORES
//------------------------------------------------------------------------
void loop() 
{ 
  static uint32_t last_time = 0; // Tempo anterior (ultima amostra)
  const uint32_t now = micros(); // Tempo atual (us)
  LeituraCom();   // a cada iteração, sem gate de tempo
  if ( (uint32_t) (now - last_time) >= TS_us ) // Devo amostar!
  {
    last_time += TS_us; // Incrementa Ts para a proxima aquisicao
    noInterrupts(); // Desabilita as interrupcoes
    for (int i=0; i<4; i++)
    {
      delta_pulsos[i] = pulsos[i]; //Variação de pulsos
      pulsos[i] = 0; // Reseta a contagem
    }
    interrupts(); // Habilita as interrupcoes
    calculo_rpm();
    Filter();
    PI_rpm();
    //controle_pwm(wteste);
    controle_pwm(total_pi);
  }
  static uint32_t ultimoPrint = millis();
  if (millis() - ultimoPrint >= 500 && 0)
  {
    ultimoPrint += 500;
    ImprimirCom();
    //delay(36);
  }
    //print_rpm(100e3);
}

//------------------------------------------------------------------------
// SETUP GERAL
//------------------------------------------------------------------------
void setup1(){

  // Portas de selecao do multiplex:
  pinMode(s0, OUTPUT); 
  pinMode(s1, OUTPUT); 
  pinMode(s2, OUTPUT); 
  pinMode(s3, OUTPUT); 

  digitalWrite(s0, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);

  analogReadResolution(10); //Fixa conversão em 10 bits
  delay(2);
  // Kicker:
  pinMode(portaKicker, OUTPUT);
  digitalWrite(portaKicker, LOW);
  //Comunicação
  delay(25);
  set2 = true;
  while( !set1 || !set2); //Serial.println("Aguardando");
}

//Valores no verde: 0: 1010 1: 1023 2: 1023 3: 1022 4: 1023 5: 1023 6: 1023 7: 1019 8: 1007 9: 1014 10: 1014 11: 1023 12: 1023 13: 1023 14: 1023 15: 1023 
//Valores frente no branco: 0: 974 1: 666 2: 695 3: 1017 4: 1014 5: 1022 6: 1021 7: 1022 8: 1007 9: 1008 10: 1008 11: 1021 12: 1022 13: 1019 14: 732 15: 761

//------------------------------------------------------------------------
// LOOP GERAL
//------------------------------------------------------------------------
void loop1() // Loop de jogo --> aqui executamos a estrategia
{
  //while(1) ImprimirMUX();
  while (okCount < 1); // Agurada a chegada do primeiro pacote valido
  Serial.println("Calibrando...");
  for (int i = 0;i<5;i++)
  {
    media_bus = MediaCircular(RobotAng);
    Serial.print("med = ");
    Serial.println(media_bus);
    delay(50);
  }
  delay(5);
  
  uint32_t flag_saindo_linha = 0; //Tempo em que o robô começa a detectar a linha
  saindo_linha = false;
  int bno_filtrada;

  while(1) // Estrategia de jogo
  {
    //TS_US2
  
    static uint32_t _last_time = micros(); // Tempo anterior (ultima amostra)
    const uint32_t _now = micros(); // Tempo atual (us)
    const uint32_t delta_t_us2 = (uint32_t) (_now - _last_time);
    
    //TS_US_FOT
    static uint32_t _last_time_fot = micros(); // Tempo anterior (ultima amostra)
    const uint32_t delta_t_us_fot = (uint32_t) (_now - _last_time_fot);

    AtualizarKicker();

    if (delta_t_us_fot >= TS_US_FOT) // Amostrador - linha
    {
      if (delta_t_us_fot < 2 * TS_US_FOT)
        _last_time_fot += TS_US_FOT;
      else
        _last_time_fot = _now;
      

      if (!saindo_linha) // Nao esta saindo da linha
      {
        saindo_linha = DetectarLinha();
        if (saindo_linha) // Acabou de detectar a linha
        {
          //Serial.println("achei linha");
          flag_saindo_linha = micros();
          //Comeca a sair da linha sem girar
          if (MODO == 1)
            SairLinhaAtaque(0.0); // Mais suave nas rotacoes
          else if (MODO == 0)
            SairLinhaDefesa(0.0);
        }
      }
      else if ((uint32_t) (_now - flag_saindo_linha) > 250e3)// Ja esta saindo da linha
      {
        saindo_linha = false;
        Desloca(0.0, 0.0, 0.0);
        //Serial.println("aqui");
      }
      else
      {
        DetectarLinha2();
        if (MODO == 1)
            SairLinhaAtaque(pd_bus/1.5); // Mais suave nas rotacoes
        else if (MODO == 0)
            SairLinhaDefesa(pd_bus/1.5);
      }
      

    }

    if ( delta_t_us2 >= TS_US2 ) // Devo amostar!
    {
      if ( delta_t_us2 < 2 * TS_US2)
        _last_time += TS_US2; // Incrementa Ts para a proxima aquisicao
      else
        _last_time = _now;
      bno_filtrada = MediaCircular(RobotAng); // Implementa a media dos valores recebidos (filtragem)
      if(yAngle != 404 && GOAL == 1){ // 
        pd_bus = PD_Cam(yAngle) * -1;
        //Serial.println("Usando gol Amarelo");
      }
      else if(bAngle != 404 && GOAL == 2){ // 
        pd_bus = PD_Cam(bAngle) * -1;
        //Serial.println("Usando gol Azul");
      } 
      else{
        pd_bus = PD_bno(media_bus, bno_filtrada);// Calcula a correcao para manter a orientacao
        //Serial.println("Usando a bussola");
        //Serial.println(saindo_linha);
      }
      ImprimirCom(); // Imprime os valores recebidos pela placa de cima
      
      /*Serial.print("pd bus: ");
      Serial.print(pd_bus);
      Serial.print(" bno filtrada = ");
      Serial.print(bno_filtrada);
      Serial.print("\n");*/
      
        //Desloca(1500.0, 0.0, pd_bus);
        if ( !saindo_linha ) // Nao esta saindo da linha --> atacar / defender 
        {
          //Desloca(1500.0, 0.0, pd_bus);
          if (MODO == 1)
            Atacar();
          else if (MODO == 0)
            Defender();
        }
    } 
  }
}
