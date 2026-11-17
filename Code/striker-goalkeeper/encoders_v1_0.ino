void Imprimir_pulsos()
{
  Serial.print("Pulsos: ");
  for ( int i = 0; i < 4; i++ )
  {
    Serial.print(pulsos[i]);
    Serial.print(" ");
  }

  Serial.print(" ");

  Serial.print("delta_Pulsos: ");

  for ( int i = 0; i < 4; i++ )
  {
    Serial.print(delta_pulsos[i]);
    Serial.print(" ");
  }
  Serial.println();
}

// Procedimento para contagem dos pulsos
void ISR_ENC1()
{
  pulsos[0]++;
}
void ISR_ENC2()
{
  pulsos[1]++;
}
void ISR_ENC3()
{
  pulsos[2]++;
}
void ISR_ENC4()
{
  pulsos[3]++;
}


void calculo_rpm() 
{
  for (int i=0; i<4; i++)
  {
    v_rpm[i] = 60000000.0* (float)delta_pulsos[i]/ ((float)TS_us*PPR);
  }
  /*for (int i=0; i<4; i++)
  {
    v_rpm[i] = 60000000.0* (float)delta_pulsos[i]/ ((float)TS_us*PPR);
    vf_rpm[i] = ((1 - ALPHA) * v_rpm_0[i]) + ALPHA * v_rpm[i];
    v_rpm_0[i] = vf_rpm[i];
    error[i] = abs(ref_rpm[i]) - vf_rpm[i];

    proportional[i] = error[i] * KP;
    integral[i] = integral_0[i] + KI * (TS_us * error[i])/1e6;
    if (integral[i] > MAX_PWM)
    {
      integral[i] = MAX_PWM;
    }
    else if (integral[i] < -MAX_PWM)
    {
      integral[i] = -MAX_PWM;
    }
    if (ref_rpm[i] > 0.0)
    {
      sinal_ref = 1.0;
    }
    
    else
    {
      sinal_ref = -1.0;
    }
    total_pi[i] = (int) ( (proportional[i] + integral[i]) * (ref_rpm[i] * sinal_ref) );
    integral_0[i] = integral[i];

  }*/
}

void Filter()
{
  for (int i=0; i<4; i++)
  {
    vf_rpm[i] = ((1 - ALPHA) * v_rpm_0[i]) + ALPHA * v_rpm[i];
    v_rpm_0[i] = vf_rpm[i];
  }
}

void Imprimir_Velocidades()
{
  Serial.print("Velocidade desejada ");
  
  for (int i =0; i<4; i++)
  {
    Serial.print(ref_rpm[i]);
    Serial.print(" ");
  }
  Serial.println();

  
  Serial.print("Velocidade atual ");
  
  for (int i =0; i<4; i++)
  {
    Serial.print(v_rpm[i]);
    Serial.print(" ");
  }
  Serial.println();

  Serial.print("Velocidade filtrada ");
  
  for (int i =0; i<4; i++)
  {
    Serial.print(vf_rpm[i]);
    Serial.print(" ");
  }
  Serial.println();

  Serial.print("PI ");
  
  for (int i =0; i<4; i++)
  {
    Serial.print(total_pi[i]);
    Serial.print(" ");
  }
  Serial.println();
}