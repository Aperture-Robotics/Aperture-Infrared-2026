//FT1 é o primeiro fototransistor
int readMux(int channel){
  int controlPin[] = {s0, s1, s2, s3};

  //loop through the 4 sig
  for(int i = 0; i < 4; i ++){
    digitalWrite(controlPin[i], (channel >> i) & 1);
  }
  delayMicroseconds(20); // tempo para estabilizar a leitura dos fototranssistores ( pode aumentar eventualmente )
  //read the value at the SIG pin
  return analogRead(SIG_pin);
}

float AnguloFot()
{
  double rx = 0.0;
  double ry = 0.0;
  for (int x=0; x<16; x++)
  { 
    rx += (double) cosList[x] * fototransistores_leitura[x];
    ry += (double) senList[x] * fototransistores_leitura[x];
  }
  float ang_fot = atan2(ry, rx)/PI * 180.0; //y e x, em graus
  return (int) -ang_fot;
}

void ImprimirMUX()
{
  for(int i = 0; i < 16; i ++)
  {
    Serial.print(i);
    Serial.print(": ");
    Serial.print(readMux(i));
    Serial.print(" ");
  }
  Serial.println();
}

bool DetectarLinha()
{
  bool det = false;
  for (int i = 0;i < 16;i++)
  {
    fototransistores_leitura[i] = readMux(i) < 920;
    det |= fototransistores_leitura[i];
  }
  zona_frente = fototransistores_leitura[0] || fototransistores_leitura[1] || fototransistores_leitura[14] || fototransistores_leitura[15] ;
  zona_esq = fototransistores_leitura[13] || fototransistores_leitura[12] || fototransistores_leitura[11] || fototransistores_leitura[10];
  zona_tras = fototransistores_leitura[9] || fototransistores_leitura[8]  || fototransistores_leitura[7]  || fototransistores_leitura[6];
  zona_dir  = fototransistores_leitura[2] || fototransistores_leitura[3]  || fototransistores_leitura[4]  || fototransistores_leitura[5];
  return det;
}

void DetectarLinha2()
{
  bool det = false;
  bool fototransistores_leitura_copia[16] = {false};

  for (int i = 0;i < 16;i++)
  {
    fototransistores_leitura_copia[i] = readMux(i) < 920;
    det |= fototransistores_leitura_copia[i];
  }

  if (det)
  {
    for (int i=0; i<16; i++)
    {
      fototransistores_leitura[i] = fototransistores_leitura_copia[i];
    }

    zona_frente = fototransistores_leitura[0] || fototransistores_leitura[1] || fototransistores_leitura[14] || fototransistores_leitura[15] ;
    zona_esq = fototransistores_leitura[13] || fototransistores_leitura[12] || fototransistores_leitura[11] || fototransistores_leitura[10];
    zona_tras = fototransistores_leitura[9] || fototransistores_leitura[8]  || fototransistores_leitura[7]  || fototransistores_leitura[6];
    zona_dir  = fototransistores_leitura[2] || fototransistores_leitura[3]  || fototransistores_leitura[4]  || fototransistores_leitura[5];
  }

  

}

void SairLinhaAtaque(float w_local)
{
  int ang_cs = AnguloFot();
  int ang_cs_wrap = Wrap180(ang_cs-180);
  /*Serial.print("ang_cs: ");
  Serial.print(ang_cs);  
  Serial.print("  ang_cs_wrap: ");
  Serial.println(ang_cs_wrap); */
  Desloca(1700.0, ang_cs_wrap , w_local);
}

void SairLinhaDefesa (float w_local)
{
  Serial.print("frente: ");
  Serial.print(zona_frente);
  Serial.print(" tras: ");
  Serial.print(zona_tras);
  Serial.print(" direita: ");
  Serial.print(zona_dir);
  Serial.print(" esquerda: ");
  Serial.println(zona_esq);
  if(zona_tras && zona_esq)
  {
    Desloca(1800.0, -60,  w_local);
  }
  
  else if(zona_tras && zona_dir)
  {
    Desloca(1800.0, 60, w_local);
  }
  
  //Detecta direita, deslocar para a diagonal esquerda
  else  if (zona_dir)
  {
    Desloca(1800.0, 60.0, w_local);
  }
  
  //Detecta frente, deslocar para tras
  else  if (zona_frente)
  {
    Desloca(1800.0, -180.0, w_local);
  }
  
  //Detecta esquerda, deslocar para a diagonal direita
  else if (zona_esq)
  {
    Desloca(1800.0, -60.0, w_local);
  }
  //Detecta tras, deslocar para frente
  else
  {
    //Serial.println("aqui");
    Desloca(1800.0, 0.0, w_local);
  }
  
}