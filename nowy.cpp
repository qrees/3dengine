#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <dir.h>
#include <string.h>

#include "allegro.h"
#include "engine1.h"

//#define xs 50
//#define ys 50
//#define xs2 2*xs
//#define ys2 2*ys



void gradient(BITMAP*buf,
              luint x1_,luint y1_,luint c1_,
              luint x2_,luint y2_,luint c2_,
              luint x3_,luint y3_,luint c3_)
{
 luint c1r=getr(c1_);luint c1g=getg(c1_);luint c1b=getb(c1_);
 luint c2r=getr(c2_);luint c2g=getg(c2_);luint c2b=getb(c2_);
 luint c3r=getr(c3_);luint c3g=getg(c3_);luint c3b=getb(c3_);
 double x1=double(x1_),x2=double(x2_),x3=double(x3_),
        y1=double(y1_),y2=double(y2_),y3=double(y3_);

 luint x_,y_;
 double d1,d2,d3,x,y;
 double d1_,d2_,d3_;
 double cr,c1r_a,c2r_a,c3r_a,
        cg,c1g_a,c2g_a,c3g_a,
        cb,c1b_a,c2b_a,c3b_a;

 luint cri,cgi,cbi;
 clear(buf);
 d1=odl(x2,y2,x3,y3,x1,y1);
 d2=odl(x3,y3,x1,y1,x2,y2);
 d3=odl(x1,y1,x2,y2,x3,y3);
 //textprintf(screen,font,buf->w+1,0,white,"d1:%f  d2:%f  d3:%f",d1,d2,d3);
 for(x=1;x<buf->w;x++)
 for(y=1;y<buf->h;y++)
 {
  if(wewn(x1,y1,x2,y2,x3,y3,x,y)==1)
  {
   d1_=odl(x2,y2,x3,y3,x,y);d1_=sqrt(d1_/d1);
   d2_=odl(x3,y3,x1,y1,x,y);d2_=sqrt(d2_/d2);
   d3_=odl(x1,y1,x2,y2,x,y);d3_=sqrt(d3_/d3);
   cr=double(c1r)*d1_+double(c2r)*d2_+double(c3r)*d3_;
   if(cr>255)cr=255;
   cg=double(c1g)*d1_+double(c2g)*d2_+double(c3g)*d3_;
   if(cg>255)cg=255;
   cb=double(c1b)*d1_+double(c2b)*d2_+double(c3b)*d3_;
   if(cb>255)cb=255;
   cri=int(cr);cgi=int(cg);cbi=int(cb);
   if(wewn(x1,y1,x2,y2,x3,y3,x,y)==1)
   putpixel(buf,int(x),int(y),makecol32(cri,cgi,cbi));
  };
 };
};

int main()
{
   int k;

   //srand(time(0));

   allegro_init();
   install_keyboard();
   install_mouse();
   install_timer();
   #ifdef debug
    plik=fopen("wys.out","w");
    fprintf(plik,"start: PROGRAM\n");
    fflush(plik);
   #endif

   int c = GFX_AUTODETECT;
   int w = 320;
   int h = 200;
   int bpp = 32;
   int go=1;

   for(;go;)
   {
    set_color_depth(8);
    set_gfx_mode(GFX_VGA, 320, 200, 0, 0);
    set_pallete(desktop_pallete);
    gfx_mode_select_ex(&c, &w, &h, &bpp);

    set_color_depth(bpp);

    if (set_gfx_mode(c, w, h, 0, 0)==0) {
     go=0;
    };
   };
   text_mode(0);
   BITMAP *buf = create_bitmap_ex(bpp,h-1,h-1);
   clear(buf);
   clock_t start,kon;
   lsint i,j;
   int xplus,yplus;
   double d;
   char*tekst=new char[10];

   #ifdef debug
    fprintf(plik,"start: inicjacja danych\n");
    fflush(plik);
   #endif
   PNKT3D*p[8];
   PNKT3D sr;sr.ustaw(0,0,3,0);
   p[1]=new PNKT3D;p[1]->ustaw(-1,-1,2,green);
   p[2]=new PNKT3D;p[2]->ustaw(-1,1,2,red);
   p[3]=new PNKT3D;p[3]->ustaw(1,-1,2,blue);
   p[4]=new PNKT3D;p[4]->ustaw(1,1,2,white);
   p[5]=new PNKT3D;p[5]->ustaw(-1,-1,4,color1);
   p[6]=new PNKT3D;p[6]->ustaw(-1,1,4,color2);
   p[7]=new PNKT3D;p[7]->ustaw(1,-1,4,color3);
   p[8]=new PNKT3D;p[8]->ustaw(1,1,4,black);

   TRI*troj=new TRI(p[1],p[2],p[3]);
   troj->dodaj(p[3],p[2],p[4]);
   troj->dodaj(p[3],p[1],p[5]);
   troj->dodaj(p[3],p[7],p[5]);
   troj->dodaj(p[3],p[7],p[4]);
   troj->dodaj(p[8],p[7],p[4]);
   troj->dodaj(p[8],p[2],p[4]);
   troj->dodaj(p[8],p[2],p[6]);
   troj->dodaj(p[1],p[2],p[6]);
   troj->dodaj(p[1],p[5],p[6]);
   troj->dodaj(p[8],p[5],p[6]);
   troj->dodaj(p[8],p[5],p[7]);
   #ifdef debug
    fprintf(plik,"end: inicjacja danych\n");
    fflush(plik);
   #endif

   SCENE*scena=new SCENE(troj,buf->w,buf->h);

    if(bpp!=32){scena->mode=m_flat;}else{scena->mode=0;}

   scena->d=2;
   scena->back=blue;
   scena->zakres(-0.5,0.5,-0.5,0.5);
   trans_view(p,8,scena->d);
   scena->policz();
   start=clock();
   for(i=0;!key[KEY_ESC];i++)
      {
       textprintf(screen,font,buf->w+1,0,white,
              "FPS:%2.5f",(double)CLOCKS_PER_SEC*i/(clock()-start));
       scena->render(buf);

       blit(buf,screen,0,0,0,0,buf->w,buf->h);
       get_mouse_mickeys(&yplus,&xplus);
       if(xplus|yplus)
         {
          trans_obrot(sr,p,8,-double(xplus)/400,double(yplus)/400,0);
          trans_view(p,8,scena->d);
          scena->policz();
         };
      };

   /*for(i=0;!key[KEY_ESC];i++)
      {
       scena->render(buf);
       itoa(i,tekst,10);
       tekst=strcat(tekst,".bmp");
       save_pcx(tekst,buf,NULL);
       //blit(buf,screen,0,0,0,0,buf->w,buf->h);
       textprintf(screen,font,buf->w+1,2,white,"Frame:%U",i);
       trans_obrot(sr,p,8,0.05,0.1,0);
       trans_view(p,8,scena->d);
       scena->policz();
      };*/
   kon=clock();
   delete tekst;
   delete scena;
   delete troj;
   for(i=1;i<=8;delete p[i],i++);
   #ifdef debug
    fprintf(plik,"end: PROGRAM\n");
    fflush(plik);
    fclose(plik);
   #endif
   destroy_bitmap(buf);
   set_gfx_mode(GFX_TEXT,0,0,0,0);
   clear_keybuf();
   do
   {}
   while(!keypressed());

   return 0;
}

