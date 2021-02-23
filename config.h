/* See LICENSE file for copyright and license details. */

/* Constants */
#define TERMINAL "st"
#define TERMCLASS "St"

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static unsigned int gappih          = 20;       /* horiz inner gap between windows */
static unsigned int gappiv          = 10;       /* vert inner gap between windows */
static unsigned int gappoh          = 10;       /* horiz outer gap between windows and screen edge */
static unsigned int gappov          = 30;       /* vert outer gap between windows and screen edge */
static int swallowfloating          = 0;        /* 1 means swallow floating windows by default */
static int smartgaps                = 0;        /* 1 means no outer gap when there is only one window */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=10" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

/* Scratchpads */
typedef struct {
	const char *name;
	const void *cmd;
} Sp;
const char *spcmd1[] = {TERMINAL, "-n", "spterm", "-g", "120x34", NULL };
const char *spcmd2[] = {TERMINAL, "-n", "spcalc", "-f", "monospace:size=16", "-g", "50x20", "-e", "bc", "-lq", NULL };
static Sp scratchpads[] = {
	/* name          cmd  */
	{"spterm",      spcmd1},
	{"spcalc",      spcmd2},
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
  /* class      instance    title       tags mask     isfloating   isterminal   noswallow   monitor */
  { "Gimp",     NULL,       NULL,       1 << 8,       0,           0,           0,          -1 },
  { TERMCLASS,  NULL,       NULL,       0,            0,           1,           0           -1 },
  { "st",       NULL,       NULL,       0,            0,           1,           1,          -1 }, /* Remove? */
  { NULL,       "spterm",   NULL,       SPTAG(0),     1,           1,           0,          -1 },
  { NULL,       "spcalc",   NULL,       SPTAG(1),     1,           1,           0,          -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

#define FORCE_VSPLIT 1  /* nrowgrid: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
   { "TTT",      bstack },
   
	{ "[@]",      spiral },  /* Fibonacci spiral */
 	{ "[\\]",     dwindle }, /* Decreasing in size right and leftward */

   { "D",        deck },    /* Master on left, slaves in monocle-like mode on right */
	{ "[M]",      monocle }, /* All windows on top of each other */

   { "|M|",      centeredmaster },          /* Master in middle, slaves on sides */
   { ">M>",      centeredfloatingmaster },  /* Same, but master floats */
      
	{ "><>",      NULL },    /* no layout function means floating behavior */
   { NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

#define STACKKEYS(MOD,ACTION)                                         \
  { MOD,   XK_j,    ACTION##stack,  {.i = INC(+1) } },                \
  { MOD,   XK_k,    ACTION##stack,  {.i = INC(-1) } },                \
  { MOD,   XK_v,    ACTION##stack,  {.i = 0 } },                      \
	/* { MOD, XK_grave, ACTION##stack, {.i = PREVSEL } }, \ */
	/* { MOD, XK_a,     ACTION##stack, {.i = 1 } }, \ */
	/* { MOD, XK_z,     ACTION##stack, {.i = 2 } }, \ */
	/* { MOD, XK_x,     ACTION##stack, {.i = -1 } }, */
     
/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "st", NULL };

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
  { "color0",           STRING,  &normbordercolor },
  { "color8",           STRING,  &selbordercolor },
  { "color0",           STRING,  &normbgcolor },
  { "color4",           STRING,  &normfgcolor },
  { "color0",           STRING,  &selfgcolor },
  { "color4",           STRING,  &selbgcolor },
  { "borderpx",         INTEGER, &borderpx },
  { "snap",             INTEGER, &snap },
  { "showbar",          INTEGER, &showbar },
  { "topbar",           INTEGER, &topbar },
  { "nmaster",          INTEGER, &nmaster },
  { "resizehints",      INTEGER, &resizehints },
  { "mfact",            FLOAT,   &mfact },
  { "gappih",           INTEGER, &gappih },
  { "gappiv",           INTEGER, &gappiv },
  { "gappoh",           INTEGER, &gappoh },
  { "gappov",           INTEGER, &gappov },
  { "swallowfloating",  INTEGER, &swallowfloating },
  { "smartgaps",        INTEGER, &smartgaps },
};

#include <X11/XF86keysym.h>
#include "shiftview.c"

static Key keys[] = {
	/* modifier                    key      function        argument */
  STACKKEYS(MODKEY,                         focus)
  STACKKEYS(MODKEY|ShiftMask,               push)
  TAGKEYS(                        XK_1,                      0)
  TAGKEYS(                        XK_2,                      1)
  TAGKEYS(                        XK_3,                      2)
  TAGKEYS(                        XK_4,                      3)
  TAGKEYS(                        XK_5,                      4)
  TAGKEYS(                        XK_6,                      5)
  TAGKEYS(                        XK_7,                      6)
  TAGKEYS(                        XK_8,                      7)
  TAGKEYS(                        XK_9,                      8)

  { MODKEY,                       XK_0,      view,           {.ui = ~0 } },
  { MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },

  /* pamixer commands to be included here... */
  
  { MODKEY,                       XK_q,      killclient      {0} },
  { MODKEY|ShiftMask,             XK_q,      quit,           {0} },
  { MODKEY,                       XK_w,      spawn,          SHCMD("$BROWSER") },
  { MODKEY|ShiftMask,             XK_w,      spawn,          SHCMD(TERMINAL "-e sudo nmtui") },
  { MODKEY,                       XK_e,      spawn,          SHCMD("$EDITOR") },

  { MODKEY,                       XK_r,      spawn,          SHCMD(TERMINAL "-e lf") },
  { MODKEY|ShiftMask,             XK_r,      spawn,          SHCMD(TERMINAL "-e htop") },

  { MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },  // tile
  { MODKEY|ShiftMask,             XK_t,      setlayout,      {.v = &layouts[1]} },  // bstack
  { MODKEY,                       XK_y,      setlayout,      {.v = &layouts[2]} },  // spiral
  { MODKEY|ShiftMask,             XK_y,      setlayout,      {.v = &layouts[3]} },  // dwindle
  { MODKEY,                       XK_u,      setlayout,      {.v = &layouts[4]} },  // deck
  { MODKEY|ShiftMask,             XK_u,      setlayout,      {.v = &layouts[5]} },  // monocle
  { MODKEY,                       XK_i,      setlayout,      {.v = &layouts[6]} },  // centeredmaster
  { MODKEY|ShiftMask,             XK_i,      setlayout,      {.v = &layouts[7]} },  // centeredfloatingmaster
  { MODKEY,                       XK_o,      incnmaster,     {.i = +1 } },
  { MODKEY|ShiftMask,             XK_o,      incnmaster,     {.i = -1 } },

  { MODKEY,                       XK_p,      spawn,          SHCMD("mpc toggle") },
  { MODKEY|ShiftMask,             XK_p,      spawn,          SHCMD("mpc pause; pauseallmpv") },
  
  { MODKEY,                       XK_bracketleft,  spawn,    SHCMD("mpc seek -10") },
  { MODKEY|ShiftMask,             XK_bracketleft,  spawn,    SHCMD("mpc seek -60") },
  { MODKEY,                       XK_bracketright,	spawn,    SHCMD("mpc seek +10") },
  { MODKEY|ShiftMask,             XK_bracketright,	spawn,    SHCMD("mpc seek +60") },  

  { MODKEY,                       XK_a,      togglegaps,     {0} },
  { MODKEY|ShiftMask,             XK_a,      defaultgaps,    {0} },
  { MODKEY,                       XK_s,      togglesticky,   {0} },
  
  { MODKEY,                       XK_d,      spawn,          {.v = dmenucmd } },
  
  { MODKEY,                       XK_f,      togglefullscr,  {0} },
  { MODKEY|ShiftMask,             XK_f,      setlayout,      {.v = &layouts[8]} },

  { MODKEY,                       XK_g,      shiftview,      {.i = -1} },
  { MODKEY|ShiftMask,             XK_g,      shifttag,       {.i = -1} },

  { MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },

  { MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
  { MODKEY,                       XK_b,      togglebar,      {0} },
  { MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
  { MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
  { MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
  { MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
  { MODKEY,                       XK_Return, zoom,           {0} },
  { MODKEY,                       XK_Tab,    view,           {0} },
  { MODKEY|ShiftMask,             XK_c,      killclient,     {0} },


  { MODKEY,                       XK_z,      incrgaps,       {.i = +1 } },
  { MODKEY,                       XK_x,      incrgaps,       {.i = -1 } },

  { MODKEY,                       XK_space,  setlayout,      {0} },
  { MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
  { MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
  { MODKEY,                       XK_period, focusmon,       {.i = +1 } },
  { MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
  { MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
 
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

