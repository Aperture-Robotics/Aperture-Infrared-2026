int Wrap180(int ang) // Funcao para manter um angulo entre [-180, 180)
{
  ang %= 360;
  if ( ang >= 180 )
    ang-=360;
  else if (ang < -180)
    ang += 360;
  return ang; 
}

int Wrap360(int ang) // Funcao para manter um angulo entre 0 e 359
{
  ang %= 360; 
  if ( ang < 0 )
    ang += 360;
  return ang; 
}

int MediaCircular(int now_ang) // Funcao para implentar uma media nos valores do BNO
{
  static int bno_filtrado = 0; // Estatico para o BNO filtrado
  static bool inicializado_bus = false; // Booleano para estado de inicializacao
  const float alpha_bus = 0.9; // Constante do filtro (mais proximo de 1 --> mais rapido, menos restritivo)
  if (!inicializado_bus) // Se ainda nao foi incializado
  {
    bno_filtrado = now_ang; // O valor filtrado e o proprio angulo atual (primeiro valor)
    inicializado_bus = true; // Atualiza o status de incializacao
    return bno_filtrado; // Retorna o valor filtrado
  }
  // Se chegou ate aqui, ja foi incializado --> calcular media
  int dif =  now_ang - bno_filtrado; // Cacula a diferenca angular entre o angulo atual e o filtrado
  // O modulo da diferenca deve ser inferior a 180 graus (menor distancia angular)
  if (dif > 180)
    dif -= 360; 
  else if (dif < -180)
    dif += 360;
  bno_filtrado +=  alpha_bus * dif; // Calcula a media ponderada por alpha_bus
  if (bno_filtrado < 0)
    bno_filtrado += 360;
  else if (bno_filtrado > 359)
    bno_filtrado -= 360;
  return bno_filtrado;
}

float PD_bno(int OFFSET, int bno_ang) // Funcao para calcular a correcao para a orientacao do robo com base no BNO
{
  int error = bno_ang - OFFSET; // Erro --> diferenca do desejado para o atual
  error = Wrap180( error ); // Normaliza para [-180, 180)
  //Serial.println(error);
  float u;
  if ( abs(error) < 2) // Erro desprezivel
  {
    error = 0;
    //last_error = 0.0;
    u = 0.0;
    return u;
  }
  float p = error * KP_BUS;
  u = p; // Calcula a correcao
  if (u > 10.0)
    u = 10.0;
  else if (u < -10.0)
    u = -10.0;
  return u; // Retorna a correcao
}

float PD_Cam(int CamAngle)
{
  float u;
  float error = 0 - CamAngle;
  float p = error * KP_CAM;
  u = p;
  return u;
}

float corrBall(){
  int VMAX = 1500;
  float u;
  float error = 0 - oAngle;
  float p = error * KP_BALL;
  u = p;
  if(u > VMAX) u = VMAX;
  else if(u < -VMAX) u = -VMAX;
  return u*-1;


}