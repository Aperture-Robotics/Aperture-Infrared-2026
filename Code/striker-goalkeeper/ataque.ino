void Atacar()
{
  float absAng = (float) abs(BallAng);

  if(oDy == 1)
    kick();
  float vmod = 1500.0;
  int absBallAng = abs(BallAng);
  if (BallDist > 1000) // bola nao vista
    vmod = 0.0;
  else
  {
    const float V_MAX = 1700.0;
    const float V_MIN = 1200.0;
    const float K = 1.6;        // agressividade do contorno
    const float OFF_MAX = 90.0; // desvio maximo (tangente)
    const float DEAD = 12.0;     // zona de ataque direto
    const float ANG_LENTO = 60.0;   // a partir deste angulo, velocidade minima
    float absAng = (float) abs(BallAng);
    float sinal = (BallAng >= 0) ? 1.0 : -1.0;
    float offset = min(K * max(absAng - DEAD, 0.0f), OFF_MAX);
    ang_distort = BallAng + sinal * offset;
    Serial.print("absBallAng = ");
    Serial.println(absBallAng);
    Serial.print(" ang_distort = ");
    Serial.println(ang_distort);
    float f = min(absAng / ANG_LENTO, 1.0);
    vmod = V_MAX - (V_MAX - V_MIN) * f;
    ang_distort = Wrap180(ang_distort); // normaliza para [-180, 180)
  }
  int abs_oAngle = abs(oAngle);

  if(abs_oAngle < 130 && oAngle != 404)
  {
    int V_MAX = 1500.0;
    int vmod_corr = corrBall();
    if(abs_oAngle < 20) 
      velocidade(-vmod_corr, V_MAX, pd_bus);
    else if(abs_oAngle < 50) 
      velocidade(-vmod_corr, V_MAX*0.6, pd_bus);
    else
      //velocidade(vmod_corr, cos(ang_distort * PI/180)*V_MAX, pd_bus);
      velocidade(-vmod_corr, -300, pd_bus);
    //velocidade(-vmod_corr, 0.0, pd_bus);
    Serial.println(vmod_corr);
  }
  else{
    Desloca(vmod, ang_distort, pd_bus);
    Serial.println("Executando outro");

  }
}