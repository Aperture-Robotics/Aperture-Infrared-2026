void print_pi(){
  
  Serial.print("P: ");
  for(int i = 0; i < 4; i++)
  {
    Serial.print(proportional[i]);
    Serial. print(" ");
  }
  Serial.print("I: ");

  for(int i = 0; i < 4; i++)
  {
    Serial.print(integral[i]);
    Serial. print(" ");
  }
  Serial.println();
  
}

void controle_pwm(int _pwm_raw[]) //funcao para controle do pwm
{
  int pwm_raw[4];
  pwm_raw[0] = _pwm_raw[0];
  pwm_raw[1] = -_pwm_raw[1];
  pwm_raw[2] = -_pwm_raw[2];
  pwm_raw[3] = _pwm_raw[3];
  for (int i=0; i<4; i++)
  {
    if (pwm_raw[i] >= MAX_PWM) //limite
    {
        pwm_raw[i] = MAX_PWM - 1; //-1para evitar problemas com o ciclo do pwm
    }
    else if (pwm_raw[i] <= -MAX_PWM)
    {
        pwm_raw[i] = -MAX_PWM + 1;
    }
     
    if (pwm_raw[i] > 0) 
    {
      digitalWrite(MA_pins[i], HIGH);
      analogWrite(PWM_pins[i], pwm_raw[i]);
    }
    else
    {
      digitalWrite(MA_pins[i], LOW);
      analogWrite(PWM_pins[i], abs(pwm_raw[i]));
    }
    }
}

void PI_rpm()
{
  for (int i=0; i<4; i++)
  {

    if( abs(ref_rpm[i]) < 21.0)
    {
      total_pi[i] = 0;
      integral[i] = 0;
      integral_0[i] = 0;
    }
    else
    {
      error[i] = abs(ref_rpm[i]) - vf_rpm[i];
      proportional[i] = error[i] * KP;
      integral[i] = integral_0[i] + KI * (TS_us * error[i])/1e6;
      if (integral[i] > INT_WINDUP) 
        integral[i] = INT_WINDUP;
      else if (integral[i] < -INT_WINDUP)
        integral[i] = -INT_WINDUP;
      if (ref_rpm[i] > 0.0)
        sinal_ref = 1.0;
      else
        sinal_ref = -1.0;
      total_pi[i] = (int) ( (proportional[i] + integral[i]) *  sinal_ref);
      if( total_pi[i] > MAX_PWM) total_pi[i] = MAX_PWM;
      else if (total_pi[i] < -MAX_PWM) total_pi[i] = -MAX_PWM;
      
      integral_0[i] = integral[i];
    }
  }
}

void Teste_Motor(){
  int w[4] = {2000, 0, 0, 0};
  while(1)
  {
    Imprimir_pulsos();

    controle_pwm(w);
    delay(2000);
    w[0] = -2000;
    controle_pwm(w);
    delay(2000);

    w[0] = 0;
    w[1] = 2000;
    controle_pwm(w);
    delay(2000);
    w[1] = -2000;
    controle_pwm(w);
    delay(2000);

    w[1] = 0;
    w[2] = 2000;
    controle_pwm(w);
    delay(2000);
    w[2] = -2000;
    controle_pwm(w);
    delay(2000);

    w[2] = 0;
    w[3] = 2000;
    controle_pwm(w);
    delay(2000);
    w[3] = -2000;
    controle_pwm(w);
    delay(2000);
    
  }
}

void print_rpm(int ts_us) // Procedimento para imprimir a velocidade (RPM)
{
  static uint32_t last_time = 0; // Tempo anterior
  const uint32_t now = micros(); // Tempo atual
  if ( (now - last_time) >= (uint32_t) ts_us )
  {
    last_time += ts_us; // Atualiza o tempo anterior
    Imprimir_Velocidades();
    print_pi();
    Imprimir_pulsos();
  }
}


void velocidade(float vx, float vy, float w) //vx e vy: mm/s. w: rad/s
{
  ref_rpm[0] = (1.0/R*(-vx/1.4142 + vy/1.4142 + w*L))*9.5493; // ***
  ref_rpm[1] = (1.0/R*(vx/1.4142 + vy/1.4142 - w*L))*9.5493; // ***
  ref_rpm[2] = (1.0/R*(-vx/1.4142 + vy/1.4142 - w*L))*9.5493; // ***
  ref_rpm[3] = (1.0/R*(vx/1.4142 + vy/1.4142 + w*L))*9.5493; // ***
}
