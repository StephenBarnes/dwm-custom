/* See LICENSE file for copyright and license details. */

// Header for keysyms like volume-change buttons.
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=12" };
static const char dmenufont[]       = "monospace:size=12";
static const char col_black[]       = "#000000";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";

static const char col_brown[] = "#241f19";
static const char col_brown2[] = "#eaddca";
static const char col_brown3[] = "#4f443a";
static const char col_brown4[] = "#b87333";
static const char col_brown5[] = "#988558";

static const char *colors[][3]      = {
	/*               fg          bg           border   */
	[SchemeNorm] = { col_brown2, col_brown,   col_brown5 },
	[SchemeSel]  = { col_gray4,  col_brown3,  col_brown4  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	//{ "Gimp",     NULL,       NULL,       0,            0,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "terminator", NULL };
static const char *slockcmd[] = { "slock", NULL }; // Lock screen

static const char *upvol[]   = { "wpctl", "set-volume", "@DEFAULT_AUDIO_SINK@", "5%+", NULL };
static const char *downvol[] = { "wpctl", "set-volume", "@DEFAULT_AUDIO_SINK@", "5%-", NULL };
static const char *mutevol[] = { "wpctl", "set-mute", "@DEFAULT_AUDIO_SINK@", "toggle", NULL };

// TODO scrot on printscreen key

static const Key keys[] = {
	/* modifier, key, function, argument */

	// Spawn terminal
	{MODKEY | ShiftMask, XK_Return, spawn, {.v = termcmd}},
	{MODKEY, XK_b, togglebar, {0}},

	// Move focus between windows
	{MODKEY, XK_t, focusstack, {.i = +1}},
	{MODKEY, XK_n, focusstack, {.i = -1}},

	// Change number of windows on master stack
	{MODKEY, XK_h, incnmaster, {.i = +1}},
	{MODKEY, XK_s, incnmaster, {.i = -1}},

	// Change width of master stack, when tiled.
	{MODKEY, XK_a, setmfact, {.f = -0.05}},
	{MODKEY, XK_o, setmfact, {.f = +0.05}},

	// Swap focused client with master window.
	// Tiled layout has 1 master window (left) and 1 stack (right).
	{MODKEY, XK_Return, zoom, {0}},

	// Go to last tag/view
	{MODKEY, XK_Tab, view, {0}},

	// Kill window.
	{MODKEY | ShiftMask, XK_c, killclient, {0}},

	// Switch layout
	{MODKEY | ShiftMask, XK_t, setlayout, {.v = &layouts[0]}}, // Tiled
	{MODKEY, XK_f, setlayout, {.v = &layouts[1]}}, // Floating
	{MODKEY, XK_m, setlayout, {.v = &layouts[2]}}, // Maximize / monocle
	{MODKEY, XK_space, setlayout, {0}}, // Toggle layout
	{MODKEY | ShiftMask, XK_space, togglefloating, {0}}, // Toggle floating for current window

	{MODKEY, XK_0, view, {.ui = ~0}}, // 
	{MODKEY | ShiftMask, XK_0, tag, {.ui = ~0}}, //

	{MODKEY, XK_comma, focusmon, {.i = -1}}, // Focus previous monitor
	{MODKEY, XK_period, focusmon, {.i = +1}}, // Focus next monitor
	{MODKEY, XK_apostrophe, focusothermon, {0}}, // Focus other monitor, and warp pointer
	{MODKEY | ShiftMask, XK_comma, tagmon, {.i = -1}}, // 
	{MODKEY | ShiftMask, XK_period, tagmon, {.i = +1}}, // 

	TAGKEYS(XK_1, 0) TAGKEYS(XK_2, 1) TAGKEYS(XK_3, 2) TAGKEYS(XK_4, 3) TAGKEYS(XK_5, 4) TAGKEYS(XK_6, 5) TAGKEYS(XK_7, 6) TAGKEYS(XK_8, 7) TAGKEYS(XK_9, 8)

	// Lock key for slock (screen locker). Lock key on keyboard is not a distinct keysym, it's seen as super+N. (Probably actually super+L, but Dvorak makes L->N.)
	{MODKEY, XK_l, spawn, {.v = slockcmd}},

	// Restart dwm with mod+shift+r -- this depends on .xinitrc being set up to run dwm while return code is 1
	{MODKEY | ShiftMask, XK_r, quit, {1}},
	// Quit dwm
	{MODKEY | ShiftMask, XK_q, quit, {0}},

	// Mod+d for dmenu
	{MODKEY, XK_d, spawn, {.v = dmenucmd}},

	// Volume
	{0, XF86XK_AudioRaiseVolume, spawn, {.v = upvol}},
	{0, XF86XK_AudioLowerVolume, spawn, {.v = downvol}},
	{0, XF86XK_AudioMute, spawn, {.v = mutevol}},

	// Clipmenu (clipboard manager) with mod+v
	{MODKEY, XK_v, spawn, SHCMD("clipmenu")},

	// TODO later consider getting fibonacci / flextile patch.
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
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
