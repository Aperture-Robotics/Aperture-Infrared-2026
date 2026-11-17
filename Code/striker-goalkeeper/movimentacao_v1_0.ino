float AngleDistortion ()
{
  float absAng = abs(BallAng);
    
  if (absAng <= 15)
  {
    ang_distort = BallAng;
  }

  
  return ang_distort;




  /*
  else if (abs(BallAng) <= 30) //Até 36º
  {
    ang_distort = 2.3 * BallAng;
  }
  
  else if (abs(BallAng) <= 45)
  {
    ang_distort = 2.6 * BallAng;
  }

  else if (abs(BallAng) <= 75)
  {
    ang_distort = 2.3 * BallAng;
  }
  else if (abs(BallAng) <= 90)
  {
    ang_distort = 2.3 * BallAng;
  }
  
  else if (abs(BallAng) <= 120)
  {
    ang_distort = 2.0 * BallAng;
  }
  
  else
  {
    ang_distort = 1.5 * BallAng;
  
  }

  if(ang_distort > 180) ang_distort -= 360;
  else if (ang_distort < -180) ang_distort += 360;
  return ang_distort;
  
  
  /*
  else if (abs(BallAng) <= 30)
  {
    ang_distort = 1.2 * BallAng;
  }
  
  else if (abs(BallAng) <= 90)
  {
    ang_distort = 1.8;
  }
  
  else if (abs(BallAng) <= 160)
  {
    ang_distort = 1.7 * BallAng;
  }
  else
  {
    ang_distort = 2 * BallAng;
  }

  if (ang_distort > 180)
  {
    ang_distort -= 360;
  }
  
  else if (ang_distort < 180)
  {
    ang_distort += 360;
  }
  
  */
}

void Desloca (float v, float teta, float w)
{
  float vx = -v * sin(teta * PI/180);
  float vy = v * cos(teta * PI/180);
  velocidade(vx, vy, w);
}
