void kick ()
{
   const uint32_t _now = millis(); // Tempo atual (ms)
  if (!isKicking && _now - lastKicking >= (uint32_t) minKickingTime)
  {
    isKicking = true;
    flag_kick = _now;
    lastKicking = _now;
    digitalWrite(portaKicker, HIGH);
  }
}

void InterrompeKicker()
{
  digitalWrite(portaKicker, LOW);
  isKicking = false;  
}

void AtualizarKicker()
{
  const uint32_t _now = millis(); // Tempo atual (us)
  const uint32_t delta_t_kicker = (uint32_t) (_now - flag_kick);

  if (isKicking && delta_t_kicker >= t_kicker) // se esta chutando e passou o tempo do chute
    InterrompeKicker();
    
}

void ManobraDefesa()
{
  const uint32_t _now = millis();
  flag_manobra = _now; // armazena o tempo de inicio da manobra
  isDefesa = true;
  Desloca(1800.0, 0.0, 0.0);
  
}
void AtualizarFB()
{
  //const uint32_t delta_t_backward = (uint32_t) (_now - flag_backward);

  if (isDefesa)
  {
    const uint32_t _now = millis(); // Tempo atual (ms)
    const uint32_t delta_t_manobra = (uint32_t) (_now - flag_manobra);
    if ( delta_t_manobra >= ( TIME_FORWARD + TIME_BACKWARD )) //se passou o tempo total o robo para
    {
      Desloca(0.0, 0.0, 0.0);
      isDefesa = false;
    }
    else if ( delta_t_manobra >= TIME_FORWARD ) //se passou o tempo de ir para frente, vai para tras
        Desloca(1600.0, -180.0, pd_bus);
    else //vai para frente
        Desloca(1600.0, 0.0, pd_bus);
  }
}

/*
void Forward ()
{
  const uint32_t _now = millis(); // Tempo atual (ms)
  if (& _now - last_time_forward >= (uint32_t) time_forward)
  {
    isForward = true;
    flag_forward = _now;
    last_time_forward = _now;

    Desloca(1800.0, 0.0, pd_bus);
  }
}

void Backward()
{
  const uint32_t _now = millis(); // Tempo atual (ms)
  if (!isBackward && _now - last_time_backward >= (uint32_t) time_backward)
  {
    isBackward = true;
    flag_backward = _now;
    last_time_backward = _now;

    Desloca(1800.0, 180.0, pd_bus);
  }
}
*/