// cd ./android
// ant debug
//
//
import hypermedia.net.*;
StringDict sensval;
StringDict sensepoch;
String ip = "192.167.189.101";
int txport = 5001;
int rxport = 50000;
StringList sensid;
StringList sensor;
StringDict sensname;
PFont font;
int ns;
int is=0;
int lastTime = millis();
int dl=500;
PImage img;
UDP udp;
boolean android=false;
//////////////////////////////  
//Line[] line1;
int nlines=6;
int y0;
int dx;
//////////////////////////////  

void setup()
{
  //img = loadImage("srt.png");
  if (android)
  {
   y0=displayHeight;
  }
  else
  {
//   size(240,320);
//   size(400,600);
//   size(1080,1920);
//   size(480,800);
   y0=height;
  }
  dx=width/6;
  
  sensid=new StringList();
  sensid.append("ta01");
  sensid.append("ua01");
  sensid.append("pa01");
  sensid.append("dn01");
  sensid.append("dm01");
  sensid.append("dx01");
  sensid.append("sn01");
  sensid.append("sm01");
  sensid.append("sx01");
  sensid.append("rc01");
  sensid.append("rd01");
  sensid.append("ri01");
  sensid.append("rp01");
  sensid.append("hc01");
  sensid.append("hd01");
  sensid.append("hi01");
  sensid.append("hp01");
  ns=sensid.size();

    
  float ts=height/(ns*2);
//  print(ts);
  font = createFont("ArialMT", ts);
  textFont(font);
  
  background(0);
  fill(255);

//////////////////////////////  
//  line1=new Line[nlines];
//  for (int i=0;i<nlines;i++)
//  {
//    line1[i]=new Line(i,dx,y0);
//  }
////////////////////////////// 
  sensval=new StringDict();
  sensepoch=new StringDict();
  sensname=new StringDict();
  for (int i=0;i<ns;i++) {
	sensval.set(sensid.get(i),"0.0");
   	sensepoch.set(sensid.get(i),"00:00");
	sensname.set(sensid.get(i),"????");
  }
  
  udp = new UDP(this);
  //udpTX.log( true );     
  udp.setBuffer(256);
  udp.loopback(false);
  udp.listen(true);	
}

public void settings() {
  size(400, 600);
}


void draw()
{
//  int delta=42; //45
  int delta=height*42/800;
  int offsetv=50+height*160/2000;
  int t0=offsetv/3;
  if (height<=320) offsetv=72;
  String epoca;
  background(0);
  int i;
  for (i=0;i<ns;i++)
  { 
    text(sensname.get(sensid.get(i))+": "+float(sensval.get(sensid.get(i))),7,offsetv+delta*i);    
    epoca=sensepoch.get(sensid.get(i));
    text(epoca,395*width/480,offsetv+delta*i); 
  }
/////////////////////////////////////////////////////////
//  i=ns-2;
//  int i0=ns-8;
//  float valore;
//  valore=float(sensval.get(sensid.get(i))) - float(sensval.get(sensid.get(i+1)));
//  text(sensname.get(i)+": "+valore,7,offsetv+delta*i0);    
//  epoca=sensepoch.get(sensid.get(i));
//  text(epoca,395*width/480,offsetv+delta*i0); 
/////////////////////////////////////////////////////////
//  for (i=ns-8;i<ns-2;i++)
//  { 
//     line1[i-(ns-8)].update(float(sensval.get(sensid.get(i)))); 
//  }
/////////////////////////////////////////////////////////  
  if(millis()-lastTime>dl)   
  { 
    String message="r "+sensid.get(is);
    udp.send(message,ip,txport);
    is++;
    if(is==ns)
    {
       is=0;
       if(dl==500)
       {
         dl=10000;
       }
    }
    lastTime = millis();
  }
//////////////////////////////////////////////////  
  String d = str(day());    
  String m = str(month());  
  String y = str(year());
  String H = str(hour());    
  String M = str(minute());  
  String S = str(second());  
  d= (d.length()<2) ? "0"+d : d;
  m= (m.length()<2) ? "0"+m : m;
  H= (H.length()<2) ? "0"+H : H;
  M= (M.length()<2) ? "0"+M : M;
  S= (S.length()<2) ? "0"+S : S;
  //image(img, 7, 12);
  text("Medicina   "+m+"/"+d+"/"+y+"  "+H+":"+M+":"+S,7,t0);
//////////////////////////////////////////////////  
  
}

void receive( byte[] data, String ip, int port )
{ 
//  "<Sensor><Id>ta01</Id><Val>32.100000</Val><Date>20140624141609</Date><Info>air temp [Â°C] (update t=60s)</Info></Sensor>"
 String message = new String( data );
 //println( "receive: \""+message+"\" from "+ip+" on port "+port );
 XML xml = parseXML(message);
 XML val_ch = xml.getChild("Val");
 String val = new String(val_ch.getContent());
 XML id_ch = xml.getChild("Id");
 String id = new String(id_ch.getContent());
 XML date_ch = xml.getChild("Date");
 String date = new String(date_ch.getContent());
 sensval.set(id,val);
 XML info_ch = xml.getChild("Info");
 String info=new String(info_ch.getContent());
 sensname.set(id,info);
 if(date.length()>11) date=date.substring(8, 10)+":"+date.substring(10, 12);
 sensepoch.set(id,date);
}

class Line
{ 
  float v;
  int dx;
  int x;
  int y0;
  float t;
  Line (int x0, int dx,int y0)
  {  
    this.x = x0*dx; 
    this.v = 0.0; 
    this.dx=dx;
    this.y0=y0;
    this.t=0.3*y0/800;
  } 
  void update(float v0)
  { 
    v = v0/30*255; 
    if(v>255) v=255;
    for (int i=0;i<=v;i++)
    {
       stroke(i*3,255-i,0);
       line(x, y0-(i*t), x+dx, y0-(i*t)); 
    }
    stroke(120);
    line(x,y0-255/3*t,x+dx,y0-255/3*t);  //10 mm
    line(x,y0-255*.666*t,x+dx,y0-255*.666*t);  //20 mm    
    line(x,y0-255*t,x+dx,y0-255*t);  //30 mm        
    line(x+dx-1,y0,x+dx-1,y0-255*t);  // vline        
  } 
} 


