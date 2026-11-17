void LeituraCom()
{
  while (Serial2.available() > 0) // Tem byte disponivel no buffer para leitura
  {
    char c = Serial2.read();
    if (c == '$') // inicio: (re)sincroniza
    {
      posCom = 0;
      descartarCom = false;
    }
    else if (descartarCom)              // fora de uma mensagem: ignora
    {
        // Ignora
    }
    else if (c == '\n')                 // fim da mensagem
    {
      msgCom[posCom] = '\0';
      descartarCom = true;
      ProcessarMensagem();
    }
    else if (posCom < TAM_MSG - 1)
    {
      msgCom[posCom++] = c;
    }
    else                                // estouro: descarta até o próximo '$'
    {
      descartarCom = true;
      errCount++;
    }
  }
}

void ProcessarMensagem()
{
  // Separa payload e checksum
  char *star = strchr(msgCom, '*');
  if (star == NULL) 
  { 
    errCount++; 
    return; 
  }
  
  *star = '\0';
  // Confere checksum
  uint8_t cs = 0;
  for (const char *p = msgCom; *p; p++) 
    cs ^= *p;
  char *fimHex;
  long csRx = strtol(star + 1, &fimHex, 16);
  if (*fimHex != '\0' || csRx != cs) 
  { 
    errCount++; 
    return; 
 }

  // Extrai os valores
  int ang, dist, bno;
  int yellow, blue, orange, dY_orange, dX_orange;
  if (sscanf(msgCom, "%d,%d,%d,%d,%d,%d,%d", &ang, &dist, &bno, &yellow, &blue, &dY_orange, &dX_orange) != 7 || bno < 0 || bno > 359)
  {
    errCount++;
    return;
  }
  if (ang > 7)
    BallAng = Wrap180(ang - 5);
  else if (ang < -7)
    BallAng = Wrap180(ang + 5);
  else
    BallAng = Wrap180(ang);
  BallDist = dist;
  RobotAng = bno;
  //yState = y1;
  yAngle = yellow;
  //ySize = y3;
  //bState = b1;
  bAngle = blue;
  //bSize = b3;
  //oState = o1;
  oAngle = dX_orange;
  oDy = dY_orange;
  okCount++;
}

void ImprimirCom()
{
  Serial.print("BallAng: ");
  Serial.print(BallAng);
  Serial.print(" BallDist: ");
  Serial.print(BallDist);
  Serial.print(" RobotAng: ");
  Serial.print(RobotAng);
  //Serial.print(" yState: ");
  //Serial.print(yState);
  Serial.print(" yAngle: ");
  Serial.print(yAngle);
  //Serial.print(" ySize: ");
  //Serial.print(ySize);
  //Serial.print(" bState: ");
  //Serial.print(bState);
  Serial.print(" bAngle: ");
  Serial.print(bAngle);
  //Serial.print(" bSize: ");
  //Serial.print(bSize);
  //Serial.print(" oState: ");
  //Serial.print(oState);
  Serial.print(" oAngle: ");
  Serial.print(oAngle);
  Serial.print(" oDy: ");
  Serial.print(oDy);
  Serial.print(" | ok: ");
  Serial.print(okCount);
  Serial.print(" erros: ");
  Serial.print(errCount);
  Serial.print(" ang_distort: ");
  Serial.println(ang_distort);
}