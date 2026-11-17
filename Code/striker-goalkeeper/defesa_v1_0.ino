void Defender()
{
  int aux_BallAng = BallAng;
  float vmod = 1500.0;
  const uint32_t _now = micros(); // Tempo atual (us)
  //const uint32_t delta_t_forward = (uint32_t) (_now - _last_time_forward);

  //static uint32_t _last_time_forward = 0; // Tempo anterior (ultima amostra)
  
  AtualizarFB();
  
  if(oDy == 1)
  {
    kick();
    if ( !isDefesa )
      ManobraDefesa();
  }
  if (isDefesa)
    return;
  
  if (aux_BallAng > 110 && aux_BallAng < 145)
  {
    aux_BallAng += 20;
    Desloca(vmod - 500.0, aux_BallAng, pd_bus);
  }
  else if (aux_BallAng < -110 && aux_BallAng > -145)
  {
    aux_BallAng -= 20;
    //Desloca(700.0, aux_BallAng, pd_bus);
    Desloca(vmod - 500.0, aux_BallAng, pd_bus);
  }

  else if (abs(aux_BallAng)>=145) //Bola atras
  { 
    if(BallAng > 0) //Bola na esquerda (anti-horario)
    {
      velocidade(700.0, -300.0,pd_bus);
      //Desloca(vmod, 90, pd_bus);

    }
    else //Bola na direita (horario)
    {
      velocidade(-700.0,-300.0,pd_bus);
      //Desloca(vmod, -90, pd_bus);
    }
  }
  else
  {
    int e_def = -aux_BallAng; // error
    if (abs(e_def) < 4)
    {
      e_def = 0;
    }
    
    float p_def = e_def * KP_def; // p = proprcional , KP = constante
    //float d_def = KD_def * (e_def - last_error);
    //0-ang bola
    float u_def = p_def; 
    if (BallDist > 140 && BallDist < 200) // caso em que a bolinha esta distante
    {
      u_def /= 1.5;
    }
    else if (abs(e_def) < 20)
    {
      u_def *= 1.5;
    }

    if ( abs(BallAng) > 62 )
      velocidade(u_def, -170.0, pd_bus);
    else
      velocidade(u_def, 0.0, pd_bus);
    
  }

  }


  