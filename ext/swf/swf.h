/*
 *	libswf.a  Version 0.99
 */
#ifndef SWFDEF
#define SWFDEF

#include <unistd.h>
#include <stdio.h>
#include <math.h>

typedef struct httprequest {
    int content_length;
    char *auth_type;
    char *gateway_interface;
    char *http_accept;
    char *http_accept_charset;
    char *http_accept_language;
    char *http_connection;
    char *http_host;
    char *http_pragma;
    char *http_referer;
    char *http_user_agent;
    char *path;
    char *query_string;
    char *remote_addr;
    char *remote_host;
    char *remote_user;
    char *request_method;
    char *request_uri;
    char *script_filename;
    char *script_name;
    char *server_name;
    char *server_port;
    char *server_protocol;
    char *server_software;
    char *tz;
    char *vserver_name;
} httprequest;

/* button types */

#define TYPE_PUSHBUTTON		(0)
#define TYPE_MENUBUTTON		(1)

/* button shape states */

#define BSHitTest       	(0x8)
#define BSDown          	(0x4)
#define BSOver          	(0x2)
#define BSUp            	(0x1)

/* button transitions */

#define OverDowntoIdle          (256)
#define IdletoOverDown          (128)
#define OutDowntoIdle            (64)
#define OutDowntoOverDown        (32)
#define OverDowntoOutDown        (16)
#define OverDowntoOverUp          (8)
#define OverUptoOverDown          (4)
#define OverUptoIdle              (2)
#define IdletoOverUp              (1)

#define ButtonEnter          	(IdletoOverUp|OutDowntoOverDown)
#define ButtonExit              (OverUptoIdle|OverDowntoOutDown)

#define MenuEnter              	(IdletoOverUp|IdletoOverDown)
#define MenuExit                (OverUptoIdle|OverDowntoIdle)

/* sound sample rates */

#define RATE_5500       (5500)
#define RATE_11000      (11000)
#define RATE_22000      (22000)
#define RATE_44000      (44000)

/* for modifyobject how */

#define MOD_MATRIX	(1)
#define MOD_COLOR	(2)

/* client functions */

void swf_openfile(char *name, float xsize, float ysize, 
			float framerate, float r, float g, float b);
void swf_closefile(void);
void swf_labelframe(char *name);
void swf_setframe(int n);
int swf_getframe(void);
void swf_showframe(void);

void swf_mulcolor(float r, float g, float b, float a);
void swf_addcolor(float r, float g, float b, float a);
void swf_removeobject(int depth);
void swf_placeobject(int objid, int depth);
void swf_modifyobject(int depth, int how);

void swf_definepoly(int objid, float coords[][2], int npoints, float width);
void swf_defineline(int objid, float x1, float y1, float x2, float y2, float width);
void swf_definerect(int objid, float x1, float y1, float x2, float y2, float width);

void swf_fontsize(float height);
void swf_fontslant(float slant);
void swf_fonttracking(float track);
void swf_getfontinfo(float *aheight, float *xheight);
void swf_fontlinewidth(float width);
void swf_definefont(int fontid, char *name);
void swf_setfont(int fontid);
float swf_textwidth(char *s);
void swf_definetext(int objid, char *str, int docenter);

void swf_startsymbol(int objid);
void swf_endsymbol(void);

/* define buttons and actions */

void swf_startbutton(int objid, int menu);
void swf_addbuttonrecord(int state, int objid, int depth);
void swf_oncondition(int transitions);
void swf_actionGotoFrame(int n);
void swf_actionGetURL(char *url, char *target);
void swf_actionNextFrame(void);
void swf_actionPrevFrame(void);
void swf_actionPlay(void);
void swf_actionStop(void);
void swf_actionToggleQuality(void);
void swf_actionStopSounds(void);
void swf_actionWaitForFrame(int frame, int skipcount);
void swf_actionSetTarget(char *target);
void swf_actionGoToLabel(char *label);
void swf_endbutton(void);

void swf_startdoaction(void);
void swf_enddoaction(void);


/*  bitmap primitives */

void swf_definebitmap(int objid, char *imgname);
int swf_getbitmapinfo(int bitmapid, int *xsize, int *ysize);


/*  geometry functions */

void swf_viewport(double xmin, double xmax, double ymin, double ymax);
void swf_pushmatrix(void);
void swf_popmatrix(void);
void swf_scale(double x, double y, double z);
void swf_translate(double x, double y, double z);
void swf_rotate(double angle, char axis);
void swf_polarview(double dist, double azimuth, double incidence, double twist);
void swf_lookat(double vx, double vy, double vz, double px, double py, double pz, double twist);
void swf_perspective(double fovy, double aspect, double near, double far);
void swf_ortho(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);
void swf_ortho2(double xmin, double xmax, double ymin, double ymax);
void swf_posround(int doit);

/* shape support */

void swf_startshape(int objid);
void swf_shapelinesolid(float r, float g, float b, float a, float width);
void swf_shapefilloff(void);
void swf_shapefillsolid(float r, float g, float b, float a);
void swf_shapefillbitmapclip(int bitmapid);
void swf_shapefillbitmaptile(int bitmapid);
void swf_shapemoveto(float x, float y);
void swf_shapelineto(float x, float y);
void swf_shapecurveto(float x1, float y1, float x2, float y2);
void swf_shapecurveto3(float x1, float y1, float x2, float y2, float x3, float y3);
void swf_shapearc(float x, float y, float r, float a1, float a2);
void swf_endshape();

/* sounds support */

void swf_soundrate(int rate);
void swf_soundcomp(int nbits);
void swf_definesound(int objid, short *samples, int nsamples);
void swf_definebuttonsound(int objid, int outid, int overid, int downid);
int swf_soundstream(short *samples, int nsamples);
void swf_startsound(int soundid);

/* cgi support */

void cgi_putmimeheader(char *str);
void cgi_getrequest(httprequest *req);
void cgi_getfullrequest(httprequest *req);

int swf_nextid(void);

#endif
