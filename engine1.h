#include <math.h>

#define luint long unsigned int
#define lsint long signed int
#define ssint short signed int
#define suint short unsigned int
#define _inline inline
#define debug
//#define double float

FILE *plik;

const luint white=makecol32(255,255,255);
const luint grey10=makecol32(26,26,26);
const luint grey20=makecol32(51,51,51);
const luint grey30=makecol32(77,77,77);
const luint grey40=makecol32(102,102,102);
const luint grey50=makecol32(128,128,128);
const luint grey60=makecol32(154,154,154);
const luint grey70=makecol32(179,179,179);
const luint grey80=makecol32(205,205,205);
const luint grey90=makecol32(230,230,230);
const luint black=makecol32(0,0,0);

const luint red=makecol32(255,0,0);
const luint green=makecol32(0,255,0);
const luint blue=makecol32(0,0,255);
const luint color1=makecol32(255,255,0);
const luint color2=makecol32(255,0,255);
const luint color3=makecol32(0,255,255);

/*
  +------------------------------------------------------------------------+
  | FUNKCJE POMOCNICZE                                                     |
  +------------------------------------------------------------------------+
*/
_inline double odl(double x3,double y3,double x2,double y2,double x1,double y1)
{
 double a,b,c,s,t,r;
 a=powi(x2-x1,2)+powi(y2-y1,2);
 b=powi(x3-x1,2)+powi(y3-y1,2);
 c=powi(x2-x3,2)+powi(y2-y3,2);
 s=4*c*a;
 t=a-b+c;t=t*t;
 return (s-t);
};

_inline suint wewn(double x1,double y1,double x2,double y2,double x3,double y3,double x_,double y_)
{
 //x_+=0.0000001,y_+=0.0000001;
 x1-=x_;y1-=y_;x2-=x_;y2-=y_;x3-=x_;y3-=y_;
 suint i=0;
 if(x1*x2<0)if(y1-(y2-y1)/(x2-x1)*x1<=0)i++;
 if(x2*x3<0)if(y2-(y3-y2)/(x3-x2)*x2<=0)i++;
 if(i!=2)if(x3*x1<0)if(y3-(y1-y3)/(x1-x3)*x3<=0)i++;
 return i;
};

void blur(BITMAP*map)
{
 BITMAP*buf=create_bitmap_ex(32,map->w,map->h);
 long unsigned int r,g,b,x,y;
 int *wsk;
 for(y=0;y<map->h-2;y++)
    {
     wsk=((int *)map->line[y]);
     for(x=0;x<map->w-2;x++)
     {
      r=getr32(((int*)map->line[y])[x])+getr32(((int*)map->line[y])[x+1])+getr32(((int*)map->line[y++])[x+2])+
        getr32(((int*)map->line[y])[x])+getr32(((int*)map->line[y])[x+1])+getr32(((int*)map->line[y++])[x+2])+
        getr32(((int*)map->line[y])[x])+getr32(((int*)map->line[y])[x+1])+getr32(((int*)map->line[y])[x+2]);y-=2;
      g=getg32(((int*)map->line[y])[x])+getg32(((int*)map->line[y])[x+1])+getg32(((int*)map->line[y++])[x+2])+
        getg32(((int*)map->line[y])[x])+getg32(((int*)map->line[y])[x+1])+getg32(((int*)map->line[y++])[x+2])+
        getg32(((int*)map->line[y])[x])+getg32(((int*)map->line[y])[x+1])+getg32(((int*)map->line[y])[x+2]);y-=2;
      b=getb32(((int*)map->line[y])[x])+getb32(((int*)map->line[y])[x+1])+getb32(((int*)map->line[y++])[x+2])+
        getb32(((int*)map->line[y])[x])+getb32(((int*)map->line[y])[x+1])+getb32(((int*)map->line[y++])[x+2])+
        getb32(((int*)map->line[y])[x])+getb32(((int*)map->line[y])[x+1])+getb32(((int*)map->line[y])[x+2]);y-=2;
      r=r/9;g=g/9;b=b/9;

      //((int *)buf_->line[y+1])[x+1]=makecol(r,g,b);
      ((int *)buf->line[y+1])[x+1]=makecol32(r,g,b);
     }
    }
 blit(buf,map,1,1,1,1,map->w-2,map->h-2);
};

_inline double difference(luint c1,luint c2)
{
 suint c1r=getr32(c1),c1g=getg32(c1),c1b=getb32(c1),
       c2r=getr32(c2),c2g=getr32(c2),c2b=getb32(c2);
 return double(abs(c1r-c2r)+abs(c1g-c2g)+abs(c1b-c2b))/765;
};

/*
  +------------------------------------------------------------------------+
  | POJEDYNCZY PUNKT W PRZESTRZENI                                         |
  +------------------------------------------------------------------------+
*/
class PNKT3D
{
 public:
  PNKT3D();
  ~PNKT3D(){/*fprintf(plik,"Usuni孤o punkt:\n");*/};
  void ustaw(double x_,double y_,double z_,luint c_){x=x_;y=y_;z=z_;c=c_;};
  void pisz(int,int);
 double x,y,z;
 double xe,ye;
 luint c;
};

PNKT3D::PNKT3D()
{
 x=0;
 y=0;
 z=0;
 c=makecol32(255,255,255);
 /*fprintf(plik,"Nowy punkt:\n");*/
};


void PNKT3D::pisz(int x_,int y_)
{
 textprintf(screen,font,x_,y_,white,"x:%f  y:%f  z:%f",x,y,z);
};

/* Transformacje punkt─ */

_inline void trans_obrot(PNKT3D psrodek,PNKT3D*pnkt[],luint i,double rx,double ry,double rz)
{
 double sinrx,sinry,sinrz,
         cosrx,cosry,cosrz;
 sincos(&cosrx,&sinrx,rx);
 sincos(&cosry,&sinry,ry);
 sincos(&cosrz,&sinrz,rz);
 double x1,y1,z1;
 for(int j=1;j<=i;j++)
 {
  pnkt[j]->x-=psrodek.x;
  pnkt[j]->y-=psrodek.y;
  pnkt[j]->z-=psrodek.z;

  x1 = cosry * pnkt[j]->x  - sinry * pnkt[j]->z;
  z1 = sinry * pnkt[j]->x  + cosry * pnkt[j]->z;
  pnkt[j]->x  = cosrz * x1 + sinrz * pnkt[j]->y;
  y1 = cosrz * pnkt[j]->y  - sinrz * x1;
  pnkt[j]->z  = cosrx * z1 - sinrx * y1;
  pnkt[j]->y  = sinrx * z1 + cosrx * y1;

  pnkt[j]->x+=psrodek.x;
  pnkt[j]->y+=psrodek.y;
  pnkt[j]->z+=psrodek.z;
 };
};

_inline void trans_move(PNKT3D*pnkt[],luint i,double px,double py,double pz)
{
 for(int j=1;j<=i;j++)
 {
  pnkt[j]->x+=px;
  pnkt[j]->y+=py;
  pnkt[j]->z+=pz;
 };
};

_inline void trans_view(PNKT3D*pnkt[],luint i,double d)
{
 double x,y,z,x1,y1,z1;
 for(int j=1;j<=i;)
 {
  pnkt[j]->xe=pnkt[j]->x/(d+pnkt[j]->z);pnkt[j]->ye=pnkt[j]->y/(d+pnkt[j++]->z);
 };
};

/*
  +------------------------------------------------------------------------+
  | POJEDYNCZY TR詷K三                                                     |
  +------------------------------------------------------------------------+
*/
class TRI
{
 public:
  TRI(PNKT3D*,PNKT3D*,PNKT3D*);
  ~TRI();
  void dodaj(PNKT3D*,PNKT3D*,PNKT3D*);
  void usun(luint);
  void wypisz();
  void pisz();
  void policz(double);
  inline double pos(double,double);
  inline luint color(double,double);

  double d1,d2,d3,c;

  PNKT3D*p1,*p2,*p3;

  double p1z,p2z,p3z;

  luint i;
  TRI*nast;
};


TRI::TRI(PNKT3D*p1_,PNKT3D*p2_,PNKT3D*p3_)                 //konstraktor
{                                                          //
 p1=p1_;p2=p2_;p3=p3_;                                     //
 nast=0;                                                   //
 i=0;                                                      //
 /*fprintf(plik,"Nowy tr▏k另:\n");*/
};                                                         //

TRI::~TRI()                                                //destructor
{                                                          //
 /*fprintf(plik,"Usuni孤o tr▏k另: nr %U\n",i);*/              //
 if(nast)                                                  //
    delete nast;                                           //
                                                           //
};                                                         //

void TRI::dodaj(PNKT3D*p1,PNKT3D*p2,PNKT3D*p3)
{
 if(!nast)
   {
    nast=new TRI(p1,p2,p3);
    nast->i=i+1;
   }
 else
   {
    nast->dodaj(p1,p2,p3);
   };
};

_inline void TRI::policz(double d)
{
 //p1->xe=p1->x/(d+p1->z);p1->ye=p1->y/(d+p1->z);
 //p2->xe=p2->x/(d+p2->z);p2->ye=p2->y/(d+p2->z);
 //p3->xe=p3->x/(d+p3->z);p3->ye=p3->y/(d+p3->z);

 d1=odl(p2->xe,p2->ye,p3->xe,p3->ye,p1->xe,p1->ye);p1z=(p1->z*p1->z)/d1;
 d2=odl(p3->xe,p3->ye,p1->xe,p1->ye,p2->xe,p2->ye);p2z=(p2->z*p2->z)/d2;
 d3=odl(p1->xe,p1->ye,p2->xe,p2->ye,p3->xe,p3->ye);p3z=(p3->z*p3->z)/d3;
 //if(nast)nast->policz(d);
};

void TRI::usun(luint nr)
{
 TRI*temp;
 if(nr==i+1)
   {
    temp=nast->nast;
    nast->nast=0;
    delete nast;
    nast=temp;
   }
 else
 if(nr<i+1){i--;};
 if(nast)nast->usun(nr);
};

void TRI::pisz()
{
 printf("--%u--  x1:%f  y1:%f  z1:%f\n",i,p1->x,p1->y,p1->z);
 if (nast){nast->pisz();};
};

_inline double TRI::pos(double xp,double yp)
{
 double d1_,d2_,d3_;

 /* d1_=sqrt(odl(p2->xe,p2->ye,p3->xe,p3->ye,xp,yp)/d1);
  d2_=sqrt(odl(p3->xe,p3->ye,p1->xe,p1->ye,xp,yp)/d2);
  d3_=sqrt(odl(p1->xe,p1->ye,p2->xe,p2->ye,xp,yp)/d3);
  return p1->z*d1_+p2->z*d2_+p3->z*d3_;*/

  d1_=sqrt(odl(p2->xe,p2->ye,p3->xe,p3->ye,xp,yp)*p1z);
  d2_=sqrt(odl(p3->xe,p3->ye,p1->xe,p1->ye,xp,yp)*p2z);
  d3_=sqrt(odl(p1->xe,p1->ye,p2->xe,p2->ye,xp,yp)*p3z);
  return d1_+d2_+d3_;
};

_inline luint TRI::color(double xp,double yp)
{
 double d1_,d2_,d3_;
 luint cr,cg,cb;

  d1_=odl(p2->xe,p2->ye,p3->xe,p3->ye,xp,yp);d1_=sqrt(d1_/d1);
  d2_=odl(p3->xe,p3->ye,p1->xe,p1->ye,xp,yp);d2_=sqrt(d2_/d2);
  d3_=odl(p1->xe,p1->ye,p2->xe,p2->ye,xp,yp);d3_=sqrt(d3_/d3);
  /*cr=int(double(getr32(p1->c))*d1_+double(getr32(p2->c))*d2_+double(getr32(p3->c))*d3_);
  cg=int(double(getg32(p1->c))*d1_+double(getg32(p2->c))*d2_+double(getg32(p3->c))*d3_);
  cb=int(double(getb32(p1->c))*d1_+double(getb32(p2->c))*d2_+double(getb32(p3->c))*d3_);*/

  cr=int(d1_*getr32(p1->c)+d2_*getr32(p2->c)+d3_*getr32(p3->c));
  cg=int(d1_*getg32(p1->c)+d2_*getg32(p2->c)+d3_*getg32(p3->c));
  cb=int(d1_*getb32(p1->c)+d2_*getb32(p2->c)+d3_*getb32(p3->c));
  
  return makecol32(cr,cg,cb);
};


/*
  +------------------------------------------------------------------------+
  | SCENA                                                                  |
  |                                                                        |
  | zawiera wska南ik do renderowanych tr▏k另─ oraz informacje o metodzie |
  | renderingu.                                                            |
  |                                                                        |
  +------------------------------------------------------------------------+
*/
class SCENE
{
 public:
  SCENE(TRI*,int,int);
  ~SCENE();
  void dodaj(double*);
  void render(BITMAP*);
  void policz();
  void zakres(double xp,double xk,double yp,double yk)
       {
        xmin=xp;ymin=yp;xmax=xk;ymax=yk;
        xplus=(xmax-xmin)/screenx,yplus=(ymax-ymin)/screeny;
       };
  luint mode;
  luint back;            // 0bit - omija wartosci <z>
                         // 2bit - siatka
                         // 3bit - flat shadeing
                         // 4bit - antialiansing
  double d;
  double xmin,ymin,xmax,ymax;
  double xplus,yplus;
 private:
 TRI*trojkaty;
 inline luint color(double,double);
 int screenx,screeny;
};
#define m_nwart_z    1
#define m_siatka     4
#define m_flat       8
#define m_blur       16
#define m_anty       16
#define anty_range   1/3

SCENE::SCENE(TRI*troj,int x_,int y_)
{
 trojkaty=troj;
 int u,v;
 screenx=x_;screeny=y_;
 /*fprintf(plik,"Nowy SCENA\n");*/
};

SCENE::~SCENE()
{
 /*fprintf(plik,"Usuni孤o SCEN沔n");*/
 trojkaty=0;
 int u;
};

void SCENE::policz()
{
 TRI*troj=trojkaty;
 for(;troj;)
       {
        troj->policz(d);
        troj=troj->nast;
       };
};

_inline luint SCENE::color(double x,double y)
{
 luint col=back;
 double minz=1.8e308,sprz;
 TRI*troj=trojkaty;
 TRI*tmp=0;

 for(;troj;)
       {
        if(wewn(troj->p1->xe,troj->p1->ye,
                troj->p2->xe,troj->p2->ye,
                troj->p3->xe,troj->p3->ye,
                x,y)==1)
        {
         sprz=troj->pos(x,y);
         if(sprz<minz)
           {
            minz=sprz;
            tmp=troj;
           };
        };
        troj=troj->nast;
       };

 if(tmp)
        if(mode&m_flat)
             col=tmp->p1->c;
           else
             col=tmp->color(x,y);
 return col;
};

void SCENE::render(BITMAP*buf)
{
 TRI*troj=trojkaty;

 luint u,v;

 double x,y;
 int y_,x_;
 luint c1,c2,c3,c4,c5,c6,c7,c8,c9,cr,cg,cb,c;

 if(mode&m_siatka)
   {
    int xp=int(screenx/2);
    int yp=int(screeny/2);
    for(;troj;)
       {
        line(buf,int(troj->p1->xe)+xp,int(troj->p1->ye)+yp,
                 int(troj->p2->xe)+xp,int(troj->p2->ye)+yp,white);
        line(buf,int(troj->p2->xe)+xp,int(troj->p2->ye)+yp,
                 int(troj->p3->xe)+xp,int(troj->p3->ye)+yp,white);
        line(buf,int(troj->p3->xe)+xp,int(troj->p3->ye)+yp,
                 int(troj->p1->xe)+xp,int(troj->p1->ye)+yp,white);
        troj=troj->nast;
       };
   }
 else
 if(mode&m_anty)
 for(x=xmin,x_=0;x<xmax;x+=xplus,x_++)
 for(y=ymin,y_=0;y<ymax;y+=yplus,y_++)
    {
     c1=color(x-xplus*anty_range,y-yplus*anty_range);
     c2=color(x,y-yplus*anty_range);
     c3=color(x+xplus*anty_range,y-yplus*anty_range);

     c4=color(x-xplus*anty_range,y);
     c5=color(x,y);
     c6=color(x+xplus*anty_range,y);

     c7=color(x-xplus*anty_range,y+yplus*anty_range);
     c8=color(x,y+yplus*anty_range);
     c9=color(x+xplus*anty_range,y+yplus*anty_range);

     cr=(getr32(c1)+getr32(c2)+getr32(c3)+
        getr32(c4)+getr32(c5)+getr32(c6)+
        getr32(c7)+getr32(c8)+getr32(c9))/9;
     cg=(getg32(c1)+getg32(c2)+getg32(c3)+
        getg32(c4)+getg32(c5)+getg32(c6)+
        getg32(c7)+getg32(c8)+getg32(c9))/9;
     cb=(getb32(c1)+getb32(c2)+getb32(c3)+
        getb32(c4)+getb32(c5)+getb32(c6)+
        getb32(c7)+getb32(c8)+getb32(c9))/9;
     ((long *)buf->line[y_])[x_] = makecol32(cr,cg,cb);
    }
 else
 for(x=xmin,x_=0;x<xmax;x+=xplus,x_++)
 for(y=ymin,y_=0;y<ymax;y+=yplus,y_++)
    {
     ((long *)buf->line[y_])[x_] = color(x,y);
    };

 if(mode&m_blur)
    blur(buf);
};


