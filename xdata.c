/*
** Astrolog (Version 5.40) File: xdata.c
**
** IMPORTANT NOTICE: The graphics database and chart display routines
** used in this program are Copyright (C) 1991-1998 by Walter D. Pullen
** (Astara@msn.com, http://www.magitech.com/~cruiser1/astrolog.htm).
** Permission is granted to freely use and distribute these routines
** provided one doesn't sell, restrict, or profit from them in any way.
** Modification is allowed provided these notices remain with any
** altered or edited versions of the program.
**
** The main planetary calculation routines used in this program have
** been Copyrighted and the core of this program is basically a
** conversion to C of the routines created by James Neely as listed in
** Michael Erlewine's 'Manual of Computer Programming for Astrologers',
** available from Matrix Software. The copyright gives us permission to
** use the routines for personal use but not to sell them or profit from
** them in any way.
**
** The PostScript code within the core graphics routines are programmed
** and Copyright (C) 1992-1993 by Brian D. Willoughby
** (brianw@sounds.wa.com). Conditions are identical to those above.
**
** The extended accurate ephemeris databases and formulas are from the
** calculation routines in the program "Placalc" and are programmed and
** Copyright (C) 1989,1991,1993 by Astrodienst AG and Alois Treindl
** (alois@azur.ch). The use of that source code is subject to
** regulations made by Astrodienst Zurich, and the code is not in the
** public domain. This copyright notice must not be changed or removed
** by any user of this program.
**
** Initial programming 8/28,30, 9/10,13,16,20,23, 10/3,6,7, 11/7,10,21/1991.
** X Window graphics initially programmed 10/23-29/1991.
** PostScript graphics initially programmed 11/29-30/1992.
** Last code change made 12/20/1998.
*/

#include "astrolog.h"


#ifdef GRAPH
/*
******************************************************************************
** Graphics Global Variables.
******************************************************************************
*/

GS NPTR gs = {
#ifdef ISG
  fFalse,
#else
  fTrue,
#endif
  fFalse, fFalse, fTrue, fFalse, fFalse, fTrue, fTrue, fFalse,
  fTrue, fTrue, fFalse, fTrue, fFalse, fFalse, fFalse,
  DEFAULTX, DEFAULTY, 0, 200, 0, 0, 0, 0.0,
  BITMAPMODE, 0, 8.5, 11.0, NULL, oCore, 1111
#ifdef PCG
  , DEFHIRESMODE, DEFLORESMODE
#endif
  };

GI NPTR gi = {
  0, fFalse, -1,
  NULL, 0, NULL, NULL, 0, 0.0, fFalse,
  2, 1, 10, kWhite, kBlack, kLtGray, kDkGray, 0, 0, 0, 0, -1, -1
#ifdef X11
  , NULL, 0, 0, 0, 0, 0, 0, 0, 0
#endif
#ifdef PS
  , fFalse, 0, fFalse, 0, 0, 1.0
#endif
#ifdef META
  , NULL, NULL, MAXMETA, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
#endif
#ifdef MSG
  , -1000
#endif
#ifdef BGI
  , -1000, fFalse, VGA, VGAHI, 1, 0
#endif
#ifdef MACG
#endif
  };

#ifdef WIN
WI NPTR wi = {
  (HWND)NULL, (HWND)NULL, (HWND)NULL, (HMENU)NULL, (HACCEL)NULL,
  hdcNil, hdcNil, (HWND)NULL, (HPEN)NULL, (HBRUSH)NULL, (HFONT)NULL,
  0, 0, 0, 0, 0, 0, 0, -1, -1,
  0, 0, fFalse, fTrue, fFalse, fTrue, fFalse, 1,
  fFalse, fFalse, fTrue, fFalse, fTrue, fFalse, kBlack, 1, 1000};

OPENFILENAME ofn = {
  sizeof(OPENFILENAME), (HWND)NULL, (HINSTANCE)NULL, NULL, NULL, 0, 1, NULL,
  cchSzMaxFile, NULL, cchSzMaxFile, NULL, NULL, OFN_OVERWRITEPROMPT, 0, 0,
  NULL, 0L, NULL, NULL};

PRINTDLG prd = {
  sizeof(PRINTDLG), (HWND)NULL, (HGLOBAL)NULL, (HGLOBAL)NULL, hdcNil,
  PD_NOPAGENUMS | PD_NOSELECTION | PD_RETURNDC | PD_USEDEVMODECOPIES,
  0, 0, 0, 0, 1, (HINSTANCE)NULL, 0L, NULL, NULL, (LPCSTR)NULL, (LPCSTR)NULL,
  (HGLOBAL)NULL, (HGLOBAL)NULL};

char szFileName[cchSzMaxFile];
char szFileTitle[cchSzMaxFile];
char *szFileTemp = szFileTempCore;
#endif

/* Color tables for Astrolog's graphics palette. */

CONST KV ARR rgbbmp[cColor] = {
  0x000000L, 0x00007FL, 0x007F00L, 0x007F7FL,
  0x7F0000L, 0x7F007FL, 0x7F7F00L, 0xBFBFBFL,
  0x7F7F7FL, 0x0000FFL, 0x00FF00L, 0x00FFFFL,
  0xFF0000L, 0xFF00FFL, 0xFFFF00L, 0xFFFFFFL};
#ifdef MSG
CONST KV rgb[cColor] = {
  _BLACK, _RED, _GREEN, _BROWN,
  _BLUE, _MAGENTA, _CYAN, _WHITE,
  _GRAY, _LIGHTRED, _LIGHTGREEN, _YELLOW,
  _LIGHTBLUE, _LIGHTMAGENTA, _LIGHTCYAN, _BRIGHTWHITE};
#endif
#ifdef BGI
CONST KV ARR rgb[cColor] = {
  EGA_BLACK, EGA_RED, EGA_GREEN, EGA_BROWN,
  EGA_BLUE, EGA_MAGENTA, EGA_CYAN, EGA_LIGHTGRAY,
  EGA_DARKGRAY, EGA_LIGHTRED, EGA_LIGHTGREEN, EGA_YELLOW,
  EGA_LIGHTBLUE, EGA_LIGHTMAGENTA, EGA_LIGHTCYAN, EGA_WHITE};
#endif
#ifdef X11
CONST char *szColorX[cColor] = {
  "black", "orangered3", "green4", "darkorange2",
  "blue4", "violet", "cyan4", "grey65",
  "grey35", "orangered1", "green1", "yellow1",
  "blue1", "pink", "cyan1", "white"};
KV rgbind[cColor], fg, bg;
#endif
#ifdef WIN
int ikPalette[cColor] =
  {-0, -1, 1, 4, 6, 3, -8, 5, -3, -2, -4, -5, -7, 2, 7, -6};
#endif

/* These are the actual color arrays and variables used by the program.      */
/* Technically, Astrolog always assumes we are drawning on a color terminal; */
/* for B/W graphics, all the values below are filled with black or white.    */

KI kMainB[9], kRainbowB[8], kElemB[4], kAspB[cAspect+1], kObjB[objMax];

/* Some physical X window variables dealing with the window itself. */

#ifdef X11
XSizeHints hint;
#if FALSE
XWMHints *wmhint;
#endif
char xkey[10];
#endif


/*
******************************************************************************
** Graphics Table Data.
******************************************************************************
*/

#ifdef STROKE
CONST char szObjectFont[oNorm+2] = ";QRSTUVWXYZ     <    a  c     b  >";
CONST char szAspectFont[cAspect+1] = "!\"#$'&%()+-       ";
#endif

CONST char * ARR szDrawSign[cSign+2] = {"",
  "ND4HU2HLGDFBR6EUHLGD2G",                /* Aries       */
  "BL3D2F2R2E2U2H2NE2L2NH2G2",             /* Taurus      */
  "BLU3LHBR7GLNL3D6NL3RFBL7ERU3",          /* Gemini      */
  "BGNDHLGDFRNEFR2EREBU3NHDGLHUENRHL2GLG", /* Cancer      */
  "BF4H2UEU2H2L2G2D2FDGH",                 /* Leo         */
  "BF4BLHNGNHEU5G2ND3U2HGND6HGND6H",       /* Virgo       */
  "BGNL3HUER2FDGR3BD2L8",                  /* Libra       */
  "BH4FND6EFND6EFD6FREU",                  /* Scorpio     */
  "BG4E3NH2NF2E5NL2D2",                    /* Sagittarius */
  "BH3BLED4FND2EU2ENF2UFERFDGLF2D2G",      /* Capricorn #1 */
  "BG4EUEDFDEUEDFDEUEBU5GDGUHUGDGUHUGDG",  /* Aquarius    */
  "NL4NR4BH4F2D4G2BR8H2U4E2",              /* Pisces      */
  "BH4RFR2ER3G3D2GDFR2EU2HL3G2DG"};        /* Capricorn #2 */

CONST char * ARR szDrawSign2[cSign+2] = {"",
  "BD8U7HU3HU2H2L2G2D2F2BR12E2U2H2L2G2D2GD3G",     /* Aries  */
  "BH6BU2FDFRFNR4GLGDGD4FDFRFR4EREUEU4HUHLHEREUE", /* Taurus */
  "", /* Gemini */
  "BG5NRLH2U2E2R2F2D2G2F2R4ER2E3BH6NE2D2F2R2E2U2H2L2H2L4GL2G3", /* Cancer */
  "", /* Leo   */
  "", /* Virgo */
  "", /* Libra */
  "BH8F2ND12E2F2ND12E2F2D12F2RE2U3NGF", /* Scorpio */
  "", /* Sagittarius  */
  "", /* Capricorn #1 */
  "BG8EUE2UEDFD2FDEUE2UEDFD2FDEUE2UEBU10GDG2DGUHU2HUGDG2DGUHU2HUGDG2DG",
  "NL8NR8BH8F3DFD6GDG3BR16H3UHU6EUE3", /* Pisces */
  ""}; /* Capricorn #2 */

CONST char * ARR szDrawObject[oNorm+5] = {
  "ND4NL4NR4U4LGLDGD2FDRFR2ERUEU2HULHL",    /* Earth   */
  "U0BH3DGD2FDRFR2ERUEU2HULHL2GL",          /* Sun     */
  "BG3E2U2H2ER2FRDFD2GDLGL2H",              /* Moon    */
  "BD4UNL2NR2U2REU2HNEL2NHGD2FR",           /* Mercury */
  "LHU2ER2FD2GLD2NL2NR2D2",                 /* Venus   */
  "HLG2DF2RE2UHE4ND2L2",                    /* Mars    */
  "BH3RFDGDGDR5NDNR2U6E",                   /* Jupiter */
  "BH3R2NUNR2D3ND3RERFDGDF",                /* Saturn  */
  "BD4NEHURBFULU3NUNR2L2NU2DGBU5NFBR6GD3F", /* Uranus #1 */
  "BD4U2NL2NR2U5NUNRLBL2NUNLDF2R2E2UNRU",   /* Neptune   */
  "D2NL2NR2D2BU8GFEHBL3D2F2R2E2U2",         /* Pluto  #1 */
  "BG2LDFEULU3NURFRFBU5GLGLU2",             /* Chiron          */
  "BD4UNL3NR3U2RE2UH2L2G",                  /* Ceres           */
  "BD4UNL3NR3UE2HUHNUGDGF2",                /* Pallas Athena   */
  "BD4UNL2NR2U4NL4NR4NE3NF3NG3NH3U3",       /* Juno            */
  "BU4DBG3NLFDF2E2UERBH2GDGHUH",            /* Vesta           */
  "BG2LGFEU2HU2E2R2F2D2GD2FEHL",            /* North Node      */
  "BG4E8BG2FD2G2L2H2U2E2R2F",               /* Lilith #1       */
  "NE2NF2NG2H2GD2F2R2E2U2H2L2G",            /* Part of Fortune */
  "U2NHNEBD4NGNFU2L2NHNGR4NEF",             /* Vertex          */
  "BH4NR3D4NR2D4R3BR2U8R2FD2GL2",           /* East Point      */
  "BG4U4NR2U3EFD7BR2NURU2HU2RDBR3ULD5RU",   /* Ascendant  */
  "BH3ER4FD2GLGLG2DR6",                     /* 2nd Cusp   */
  "BH3ER4FD2GNL3FD2GL4H",                   /* 3rd Cusp   */
  "BH4R2NR2D8NL2R2BR4NUL2U8R2D",            /* Nadir      */
  "BG3FR4EU2HL5U4R6",                       /* 5th Cusp   */
  "BE3HL4GD6FR4EU2HL4G",                    /* 6th Cusp   */
  "BH4D8REU6HLBF7DRU2HU2RDBG4NRU3NRU2R",    /* Descendant */
  "BL2GD2FR4EU2HNL4EU2HL4GD2F",             /* 8th Cusp   */
  "BG3FR4EU6HL4GD2FR4E",                    /* 9th Cusp   */
  "BG4U8F2ND6E2D8BR4NUL2U8R2D",             /* Midheaven  */
  "BH3ED8NLRBR2RNRU8G",                     /* 11th Cusp  */
  "BG4RNRU8GBR4ER2FD2GLG2D2R4",             /* 12th Cusp  */
  "BH4BRFDG2DR8BG3UNL2NR2U5LUEFDL",         /* Cupido    */
  "BENUNL2NR2D3ND2NR2L2H2U2E2R4",           /* Hades     */
  "BU4NG2NF2D7NDBLHLBR6LGL2GLBR6LHL",       /* Zeus      */
  "BU2D3ND3NR2L2BH2UE2R4F2D",               /* Kronos    */
  "U3NLR2NRD3NL2NR2D4NRL2NLU4L4UEUH",       /* Apollon   */
  "BUNU2NL2NR2D2ND3LHU2ENHR2NEFD2GL",       /* Admetos   */
  "G2DGR6HUH2U4NG2F2",                      /* Vulcanus  */
  "ND4U4BL3DF2R2E2UBD8UH2L2G2D",            /* Poseidon  */
  "BD2D0BU6NG2NF2D4LGD2FR2EU2HL",           /* Uranus #2 */
  "BL3R5EU2HL5D8R5",                        /* Pluto  #2 */
  "UERHL2G2D2F2R2ELHU",                     /* Lilith #2 */
  "BH2LHEFD2GD2F2R2E2U2HU2EFGL"             /* Lilith #3 */
  };

CONST char * ARR szDrawObject2[oNorm+5] = {
  "ND8NL8NR8U8L2GLG3DGD4FDF3RFR4ERE3UEU4HUH3LHL2", /* Earth */
  "U0BU8L2GLG3DGD4FDF3RFR4ERE3UEU4HUH3LHL2",       /* Sun   */
  "BG6E3UEU2HUH3E2R4FRF3DFD4GDG3LGL4H2",           /* Moon  */
  "", /* Mercury */
  "", /* Venus   */
  "BELHL4G3D4F3R4E3U4HUE7ND5L5",      /* Mars    */
  "BH6BRRF2D2GDGDGDGDR10ND2NR4U12E2", /* Jupiter */
  "", /* Saturn  */
  "BD4LGD2FR2EU2HLU6NU2NR4L4NU4D2G2BU10NF2BR12G2D6F2",     /* Uranus #1 */
  "BD8U4NL4NR4U10NU2NR2L2BL3LNU2NLD2FDFRFR4EREUEU2NLNRU2", /* Neptune   */
  "D4NL4NR4D4BU16LGD2FR2EU2HLBL6D4FDFRFR4EREUEU4",         /* Pluto  #1 */
  "BG4LGD2FR2EU2HLU7RF2RF2RFBU10GLG2LG2BLU5",     /* Chiron        */
  "BD8U2NL6NR6U4R3E3U4H3L4G2",                    /* Ceres         */
  "BD8U2NL6NR6U2E4HUHUHUHNUGDGDGDGF4",            /* Pallas Athena */
  "BD8U2NL4NR4U8NL7NR7NE5NF5NG5NH5U6",            /* Juno          */
  "BU8D3BG5NL3DF2DF2DFEUE2UE2UR3BH4GDG2DGHUH2UH", /* Vesta         */
  "", /* North Node      */
  "", /* Lilith #1       */
  "", /* Part of Fortune */
  "", /* Vertex          */
  "", /* East Point      */
  "BG8U8NR4U6E2F2D14BR4NHREU3HLHU3ERFBR6HLGD8FRE", /* Ascendant */
  "", /* 2nd Cusp   */
  "", /* 3rd Cusp   */
  "BH8R4NR4D16NL4R4BR8BUNUGL3HU14ER3FD", /* Nadir */
  "", /* 5th Cusp   */
  "", /* 6th Cusp   */
  "BH8D16R2E2U12H2L2BF14BGFREU3HLHU3ERFBG9NR3U5NR3U5R3", /* Descendant */
  "", /* 8th Cusp   */
  "", /* 9th Cusp   */
  "BG8U16F4ND12E4D16BR8BUNUGL3HU14ER3FD", /* Midheaven */
  "", /* 11th Cusp  */
  "", /* 12th Cusp  */
  "", /* Cupido    */
  "", /* Hades     */
  "", /* Zeus      */
  "", /* Kronos    */
  "", /* Apollon   */
  "", /* Admetos   */
  "", /* Vulcanus  */
  "", /* Poseidon  */
  "", /* Uranus #2 */
  "", /* Pluto  #2 */
  "", /* Lilith #2 */
  ""  /* Lilith #3 */
  };

CONST char * ARR szDrawHouse[cSign+1] = {"",
  "BD2NLNRU4L", "BHBUR2D2L2D2R2", "BHBUR2D2NL2D2L2",
  "BHBUD2R2NU2D2", "BEBUL2D2R2D2L2", "NLRD2L2U4R2",
  "BHBUR2DG2D", "NRLU2R2D4L2U2", "NRLU2R2D4L2",
  "BH2NLD4NLRBR2U4R2D4L2", "BH2NLD4NLRBR2RNRU4L", "BH2NLD4NLRBR2NR2U2R2U2L2"};

CONST char * ARR szDrawHouse2[cSign+1] = {"",
  "BD4NL2NR2U8G2", "BH2BUER2FD2G4DR4", "BH2BUER2FD2GNLFD2GL2H",
  "BH2BU2D4R3NU4NRD4", "BE2BU2L4D4R3FD2GL2H", "NL2RFD2GL2HU6ER2F",
  "", "NRLHU2ER2FD2GFD2GL2HU2E", "NR2LHU2ER2FD6GL2H",
  "BH4NG2D8NL2R2BR5HU6ER2FD6GL2", "BH4NG2D8NL2R2BR4R2NR2U8G2",
  "BH4NG2D8NL2R2BR4NR4UE4U2HL2G"};

CONST char * ARR szDrawAspect[cAspect+3] = {"",
  "HLG2DF2RE2UHE4",                        /* Conjunction      */
  "BGL2GDFREU2E2U2ERFDGL2",                /* Opposition       */
  "BH4R8D8L8U8",                           /* Square           */
  "BU4GDGDGDGDR8UHUHUHUH",                 /* Trine            */
  "BLNH3NG3RNU4ND4RNE3F3",                 /* Sextile          */
  "BG4EUEUEUEUNL4NR4BDFDFDFDF",            /* Inconjunct       */
  "BH4FDFDFDFDNL4NR4BUEUEUEUE",            /* Semisextile      */
  "BE4G8R8",                               /* Semisquare       */
  "BD2L3U6R6D6L3D2R2",                     /* Sesquiquadrature */
  "F4BU3U2HULHL2GLDGD2FDRFR2E3",           /* Quintile         */
  "BD2U3NR3NU3L3BD5R6",                    /* Biquintile       */
  "BU2D3NR3ND3L3BU5R6",                    /* Semiquintile     */
  "BH3R6G6",                               /* Septile      */
  "BR3L5HUER4FD4GL4H",                     /* Novile       */
  "BF2UHL2GFR3DGL3BE6LNLU2NRLBL4LNLD2NLR", /* Binovile     */
  "BL2R4G4BE6LNLU2NRLBL4LNLD2NLR",         /* Biseptile    */
  "BL2R4G4BE6L7NLU2NLR3ND2R3ND2R",         /* Triseptile   */
  "BF2UHL2GFR3DGL3BU6LNLU2NLRBR2F2E2",     /* Quatronovile */
  "BU4BLD8BR2U8",                          /* Parallel       */
  "BU4BLD8BR2U8BF3BLL6BD2R6"};             /* Contraparallel */

CONST char * ARR szDrawAspect2[cAspect+3] = {"",
  "BELHL4G3D4F3R4E3U4HUE7",                /* Conjunction */
  "BG3HL2G2D2F2R2E2U2HE6HU2E2R2F2D2G2L2H", /* Opposition  */
  "", /* Square           */
  "BU8GDGDGDGDGDGDGDGDR16UHUHUHUHUHUHUHUH",     /* Trine       */
  "BU8D16BL8BU2E3RE3R2E3RE3BL16F3RF3R2F3RF3",   /* Sextile     */
  "BG8EUEUEUEUEUEUEUEUNL8NR8BDFDFDFDFDFDFDFDF", /* Inconjunct  */
  "BH8FDFDFDFDFDFDFDFDNL8NR8BUEUEUEUEUEUEUEUE", /* Semisextile */
  "", /* Semisquare       */
  "", /* Sesquiquadrature */
  "BFF7BU6U4HUH3LHL4GLG3DGD4FDF3RFR4E6", /* Quintile */
  "", /* Biquintile   */
  "", /* Semiquintile */
  "", /* Septile      */
  "", /* Novile       */
  "", /* Binovile     */
  "", /* Biseptile    */
  "", /* Triseptile   */
  "", /* Quatronovile */
  "", /* Parallel        */
  ""}; /* Contraparallel */

CONST char * ARR szDrawCh[128-32+1] = {"",
  "BR2D4BD2D0", "BRD2BR2U2", "BD2R4BD2L4BFU4BR2D4", "BR2D6BENL3EHL2HER3",
  "RDLNUBR4G4BR4DLUR", "BD2NF4UEFDG2DFRE2", "BR2DG", "BR3G2D2F2", "BRF2D2G2",
  "BD2FNGRNU2ND2RNEF", "BD3R2NU2ND2R2", "BD5BR2DG", "BD3R4", "BD6BRRULD",
  "BD5E4", /* Special Characters */

  "BDD4NE4FR2EU4HL2G", "BFED6NLR", "BDER2FDG4R4", "BDER2FDGNLFDGL2H",
  "D3R3NU3ND3R", "NR4D3R3FDGL2H", "BR3NFL2GD4FR2EUHL3", "R4DG4D",
  "BDDFNR2GDFR2EUHEUHL2G", "BD5FR2EU4HL2GDFR3", /* Numbers */

  "BR2BD2D0BD2D0", "BR2BD2D0BD2G", "BR3G3F3", "BD2R4BD2L4", "BRF3G3",
  "BDER2FDGLDBD2D0", "BF2DFEU2HL2GD4FR2", /* Special Characters */

  "BD6U4E2F2D2NL4D2", "D6R3EUHNL3EUHL3", "BR3NFL2GD4FR2E", "D6R2E2U2H2L2",
  "NR4D3NR3D3R4", "NR4D3NR3D3", "BR3NFL2GD4FR2EU2L2", "D3ND3R4NU3D3",
  "BRRNRD6NLR", "BD4DFR2EU5", "D3ND3RNE3F3", "D6R4", "ND6F2NDE2D6",
  "ND6F4ND2U4", "BDD4FR2EU4HL2G", "R3FDGL3NU3D3", "BDD4FRENHNFEU3HL2G",
  "ND6R3FDGL2NLF3", "BR3NFL2GDFR2FDGL2H", "R2NR2D6", "D5FR2EU5",
  "D2FDFNDEUEU2", "D6E2NUF2U6", "DF4DBL4UE4U", "D2FRND3REU2",
  "R4DG4DR4", /* Upper Case Letters */

  "BR3L2D6R2", "BDF4", "BRR2D6L2", "BD2E2F2", "BD6R4", "BR2DF", /* Symbols */

  "BF4G2LHU2ER2FD3", "D5NDFR2EU2HL2G", "BF4BUHL2GD2FR2E", "BR4D5NDGL2HU2ER2F",
  "BD4R4UHL2GD2FR3", "BD3RNR3ND3U2ERF", "BD8R3EU4HL2GD2FR2E", "D3ND3ER2FD3",
  "BR2D0BD2D4", "BR2D0BD2D5GLH", "D4ND2REREBD4HLH", "BR2D6",
  "BD2DND3EFNDEFD3", "BD2DND3ER2FD3", "BD3D2FR2EU2HL2G", "BD2DND5ER2FD2GL2H",
  "BR4BD8U5HL2GD2FR2E", "BD2DND3ER2F", "BD6R3EHL2HER3", "BR2D2NL2NR2D4",
  "BD2D3FRE2NU2D2", "BD2DFDFEUEU", "BD2D3FENUFEU3", "BD2F2NG2NE2F2",
  "BD2D3FR2ENU3D2GL3", "BD2R4G4R4", /* Lower Case Letters */

  "BR3GDGFDF", "BR2D2BD2D2", "BRFDFGDG", "BFEFE", "BD6R4",
  "BR2FGHE"}; /* Symbols */

CONST char * ARR szWorldData[62*3] = {
"-031+70",
"LLRRHLLLLDULLGLLLDULGLLLGLRREDEGGLGGLGLGLLGDRLDRLFFRRERFDFRRREUEEHLUERERUERR\
FGLGLDDFRRRRREFRLGLLLLLGEFDLHGDDLGHLGLLHGLHURDLRRELLLRHUGLDFDLGLLFHGGLGLLLDLL\
LDRRFFDDGLLLLLLGDFGDDRRFRERREEUEREUEFRRERRFFFRFRDDLLLLRFRUREURULHLHHHEF",
"5EUROPE",
"+006+50", "RRERRRRUELLUHHLLREULLELLDGHDUFDEGRDRRLFDLLRGRRGGL", "5ENGLAND",
"+008+55", "GLFGRRREUULL", "5IRELAND",
"+023+64", "RRFRERRREHLLLLLGHLLRFLLRFL", "5ICELAND",
"-011+80", "DDURFRERLGRRLLFRRREEFRRRLHGELLLHRRFRRRRERLLLLLLLLLLLDHGULLL",
"5SVALBARD",
"-014+45",
"FRFRFDDFRDRRLLFRURFHHUERRRRRHUUEERRRRGRDERRLHLRRERRGGRFRFFGLLLLHLLLLGLLDLLLF\
GRFFRERFRERDDDGDGLLDFFEUDDFFDFFDDFFFDFDDDRRERRERRRUERRERURUEEHHLHUGGLLLUUGUHU\
HURRFFRFRRRDRRFRRRRRRRF",
"5MIDDLE EAST",
"-009+41", "DDRUULEUGD", "5SARDINIA",
"-024+35", "RRLL", "5CRETE",
"-032+35", "RRLL", "5CYPRUS",
"-052+37", "LLHUURHUHUHERERRRDDLLLFFDDURFLLDFDDL", "0CASPAIN SEA",
"-060+44", "LLUEERDFLDL", "0ARAL SEA",
"-068+24",
"FRGFRREDDDDDFDFDDFDDFERUEUUUUEEEEEREURRREFDFRDDDDRREFDDFDDGDDRFDDFDFFRUHUUHH\
HULUEUUURDRFDFRDEEREUUUHHHUUEERRDDEURRERREREEEUEULLREUHUHLEERRHLGLULUREERDLDR\
ERRFGRFDGRRREUHHUREUE",
"6ASIA S",
"-140+36",
"DEUUEUHURREREEGLLHHDDGLDRGDDGGLGLLLGGLDLRDFEUHRRGEERDLLRGLRERRERRE",
"6JAPAN",
"-121+25", "GDFUEUL", "6TAIWAN",
"-080+10", "DDDDREUHH", "6SRI LANKA",
"-121+18", "LDDDRDDRHRRFFDDDLLEHDULRHDFDDGERDDREUUULUUHHLHEUUL",
"2PHILIPPINES",
"-131+43",
"EFREEREEEUUUEUHLLUDLULEERERERRRRRRERRFLRRRRLUERERRRDRERURRGDLGLGLGLGGDDFDFEU\
RRUERUURULEEREDERRFRERERRRERRHLHLRRRREURDRRFRFRUURRHLLLDHHLLHLLHLLLLLLLDLLHRL\
LLLLLLGHULLLLLLLLLLULLLGL",
"6SIBERIA",
"-145+71",
"RELLRHLLLLGDHGHLLLLGLLHUHLLLLLDLLLLHLLLLLDULUDLGLLLLRRERERRRELHLLLLLLLELLLLG\
DLLLLLUDLLLLLGLLLDLLLLLLLDFRDDHELLLLLLDRRLLHUDLGFGRRRRFRLHLLDGLGLLHRRREUHUUUL\
LGGLDRFGHLLLHLLLLRFGHLGLLLULGLLLGLLHRHLDDDLLLLDLLLFLLHUHLRRFRRRREHLLHLLLHLLL",
"6RUSSIA",
"-143+54", "GDDDDDDDEDUUURUUHUU", "6SAKHALIN",
"-180+72", "GRRRRULLL", "6WRANGEL I.",
"-137+76", "DRRRRRRRELLLLLLLL", "6SIBERIAN I.",
"-091+80", "FERDRRRRRRULLLLLRRULLLLGL", "6SEVERNAYA",
"-101+79", "GRRRRELLLL", "6ZEMLYA",
"-068+77", "LLGLLLLLLGLLGGLGLRFRRRRLHERERERRRERRRREL", "6NOVAYA",
"+123+49",
"FGULLFDDDGFDDDFFDFRFRFDFFFDLFFRDFFEHHHHUHHUFRDFFFRDFFFDFGFRFRFRRFRRRRFFRRFRF\
FDRFFRFEUUGLHHUUEUHLLLLLEUUEULLLGDLLGLHHUHUUUEHEERERRFRRHRREFRRFDFDFEUUHUUUEE\
RERUUUHFDEUHFEURRRELUERRE",
"4NORTH AMERICA S",
"+113+42", "FH", "0SALT LAKE",
"+156+20", "DRULHLHL", "4HAWAII",
"+085+22", "RERFRRFRGRRRRHLHLHLLLLLG", "4CUBA",
"+070+18", "RRHHLLLFLLLFRRRRRR", "4HAITI",
"+078+18", "RRHLLF", "4JAMAICA",
"+066+18", "ELLDR", "4PUERTO RICO",
"+078+24", "UD", "4NASSAU",
"+067+45",
"REFLGDERERREHDLLLHUELLLGLGLREEERRRRRRREERRGGDGRRRFEFUUHLLLEUUHHGLRELLHHUHHHD\
GLGHHULLHLLLLLDFGFDDGLLFDDGHHUULLLLHLLHLLLUHUUEREEREERRRREUUHLLLDDGHULLLHLUHL\
GDRFGGULLLLLLLLLHLLGFLHLLLLLRHLLLLLHLLLLLLHGLLLLGUGLLLHLL",
"4CANADA",
"+088+49",
"LGLGRRRRRRRFLLLGRGDDREUURUFRGRFGFERERREEREERLGGLGLLLGRLLGLEUERHLLLHULHL",
"0GREAT LAKES",
"+117+61", "REHRFRRERGLGLLLL", "0SLAVE LAKE",
"+125+66", "RRERRRGREDLFHGLLLERLLLL", "0BEAR LAKE",
"+097+50", "UULHURFDFG", "0LAKE WINNIPEG",
"+090+72",
"FRRLLFRRRRRRRRRRFRRGLLGRREEFRFLGLFLLLLFRERFRFRRFRRHLHFRRRUHLHRRFRURELLHLLLHR\
RHLHLHGHLHLLGLLEHFRRRHLLLLLLGLDFHLUELLGG",
"4BAFFIN I.",
"+125+72",
"RFRREERRRLLGFFRRRRRLLLLLFRRRRRRRREFRRRRHRRLHLHHLRRULGLFLHLDLLULLLLHLLLLLLLDG",
"4VICTORIA I.",
"+141+70",
"LLLLLLLLHGLHLLLHGLLGLLGLLDRRFRRDLLLULGLLFRRRRRRDLGLLGFDRRRDRRRRRGGGLLGLLGGLL\
RRERERRRERREERRELEERRRLLGDRERRURRFRRRRRFRRFUDRUDDHFDURDURLURDDLFRULURDHFFRGFE\
GRFFRFRFLHLHLFFRFE",
"4ALASKA",
"+045+60",
"REUEREUERRRRERERRRERRRRERLLLLLLHRRRGERHFRRRRHLUDLLHLRERFRERLEUHRRHLEERLLURRR\
RRRRRELLLLLLLLLLGLLLRERHGLRELLLLLLLELLLLLLLLLLGLLLLLLGLLLLLLGLULLLLLLLFRLLLLL\
GLRRRGLLLLLLLGRRRRRRRGLLLLRRFRRRRRRRRRRFDFDLFREFRDLLLDERRFGLLGFFDRFFFRRRF",
"4GREENLAND",
"+080+10",
"DRFDFDDGGGDDGRDGDDFFDFDFFDFFRFFFDDDDDDGDDDDGDDDDGDGFGDDDEUDDDGUDDLDRGDDDFDFR\
FRRFERRLHLUHUURUEELHEREURULURREURREREUHUUDFRREEEEEUEUUEERERRREUEUEUUUUUEEEEUU\
UHLHLHLLLLHLHLGEHLGEUHUUHLHLLLHHLHULEDLLELLGHLLHLGDDHUELLGLGDGHHL",
"3SOUTH AMERICA",
"+060-51", "LDRRELL", "3FALKLAND ISLANDS",
"+092+00", "FUL", "3GALAPAGOS I.",
"-032+32",
"LLGLHLLLLHLGDGHLLHHLLHLEUULLLLLLLLLGLGLLLLHDGLGDGDGGLDGGGDGDFDDDDGDDFFFFDFRF\
FRRRRRRRRERERRFFRRFFDDDGDFFFDFDDDFDGDGDDDFDFDFDDDFDFDFDDFFERRRRREEEEEEEUUEREU\
UHUEEEREEUUUUHUUUHUEUEEEEEREEUEUEEUUULLLLGLLHUHHLHUHHUUHHUUHUHHUU",
"1AFRICA",
"-049-12", "DGGGLGDDDDGDDFFREUEUEUUUEUUUUH", "1MADAGASCAR",
"-032+00", "DDDREUELLL", "0LAKE VICTORIA",
"-014+14", "LRFLU", "0LAKE CHAD",
"-124-16",
"LGDGGLGLLGLDDDGFDDFDFDGFRRRERRRRURERRRRRRRFFFEEDDRFDFRFREFRERRUUEUEEUUUUUUUH\
HHHHHHUUHHHUULDDDDGDGHLHLHEUELLLHLFLLULDRGDDLLHLGG",
"2AUSTRALIA",
"-173-35", "FFDGFDREURULHHHL", "2NEW ZEALAND N",
"-174-41", "LLDGLGLGGRFREEUREEU", "2NEW ZEALAND S",
"-145-41", "DFRRUUUDLLL", "2TASMANIA",
"-178-17", "GRRURUGDH", "2FIJI",
"-130+00", "FRFRLGFEFRFRFDGRRFRRUERFFFRRRLHHHHRHLHHLHLLHGGLHUHLGH",
"2NEW GUINEA",
"-115-04", "RUUEEURHUUEHHGGGGLLDDHLDDFDDRRDERF", "2BORNEO",
"-095+06", "DFFFFFFDFFFFRUUUHFRHLHLUHHHHHLLH", "2SUMATRA",
"-106-06", "GRFRRRRRRFRRHLHLLLLLHL", "2JAVA",
"-120+00", "DGDDRDFHUEDFRHUHREFHLGHURRRRELLLLG", "2CELEBES",
"+000-70",
"ULDLLLLLLLLGLLGLLLGLLGLLLLGLGLLGLLLLGLLLLLHLGLLLLLHLLLLLHLLLLHLLUERLEUUUUUUE\
ERRRULLGLLLLGLGGLLLDRUDRDLGHLLGLLFGRRFLLLLLLLDHLLLLHLLLLLGLLLLHLLLLLLLGRFDLLL\
ULLLGHLLLLLLLLLLHGHLLGLLLLLLLGLLLLLLLLLLLGLLLGLLLLLLLLGLLLLLLLLLLLLLLLLLLLLL",
"7ANTARCTICA W",
"+180-78",
"LLLLLLLHLLGHLLGHLUEERRERREHLLLLHLLLLLLHLLLLLLLLLLLHLHLLLLLHLLULDLLLLLDLLHLLL\
LGHFLLLLLHLLLLLLGLHLLHLGLLLLHLGLLGLLLULLLGLLHDFLLLGLGLLLELLLLHLLLLLLLLLLHLLLH\
LLLLGGHGHGLLLGLDLLLLHLLGHGLLLLLLLLLLLLLLHLGLLLLLLLLLLLLLL",
"7ANTARCTICA E",
"", "", ""};

#ifdef CONSTEL
CONST char * ARR szDrawConstel[cCnstl+1] = {"",
"550210+51DDd3r8d2Rr7d2Rr3Dd5l2d3r10uru6rUu2Rr2ur4u2RrUUu3Ll7d2l3DdLl5d2Lu2l4\
Uul8Dd2Ll3Uul7", /* Andromeda */
"660913-25d2Ll5Dl5d2l4d4LlDRRr8Uu5l6", /* Antila */
"561804-68DDd3RRRRRr2Uu9LLLLl3Uu2Ll4", /* Apus */
"362213+02Dd3Ll14DDd5RRrUUur7Dd4Rr6UUu2Ll9ul3dLl13", /* Aquarius */
"562003+16Ddl3d7l3Dd9r7Dd2RRUu6r5Uu2l4u4r3Uu2l3u7Lld2l13dLl3", /* Aquila */
"641803-45Dd7Rr5Dd8Ru3rur2u3r3UUu5LLl3", /* Ara */
"560307+31DDrd9RRr3Uu6Llu2l7UuLl7", /* Aries */
"650604+56d2l6Dl4d6Ll7Dd5Rr5Dd2RuRr2Ur4u6l3UUu3Ll3u3Ll4", /* Auriga */
"431504+55d2l8DDdr4d7r4Dd5rDDd2RRr6UUu8LlUlUUu5Ll4", /* Bootes */
"560501-27DDd3Rr2d3r5d3r4Uu3l5Ul2u3Ll", /* Caelum */
"751407+86DdRr6d3RRr6u3Rr2UuRr9Dd7Rr12DDRr12u2Rr11Dd4Rr12d3RRr8u2r2u2rUu8l6Uu\
7lULLl6u5LLLl5uLLLLLLl7", /* Camelopardalis */
"550906+33DDDd3Rr14URrULu8l2Uu3Ll6", /* Cancer */
"551309+52Dd2Ll2DDrd2Rr9UuRr8u2r5Uu4lUu2Ll9", /* Canes Venatici */
"550707-11DDd3Rr12UUu9Ll7", /* Canis Major */
"660714+13DLld3l2DRr10ur3Uu2l7ul5", /* Canis Minor */
"562114-09DDd5r7d2Rr13UUul7Dd4Ll7Uul7", /* Capricornus */
"360804-51d2l4d2l5d2LLLl5DDd5RRRUu6RRr2Uu2r4u3r6u2r2u2LLl4", /* Carina */
"440310+77Dd2r6Dd3Rr6u2Rdr3d3r4d4r4DdRd2r10u2Rr5Uur3u2RrULl4u4l6u3Ll6Uu7LLLl1\
0", /* Cassiopeia */
"551501-30Dd3Rr11Dd5l6Dd4RRrUu5RrDd4r9Uu3r4UUu4Ll5u2l5ULLLl", /* Centaurus */
"850805+88d3RRRr9DRRr4d3RRRr9Dd3Rr5d3r6DRrd3r7dr3d3r3u2RRr5UurdRu2l7u5l3Uu5r9\
Uul12u5LLl14u2LLLLLLLLLLl5", /* Cepheus */
"560306+11DDdRr4DDd4Rr4dRRrUUu4Ll7Uu3LLl2UuLl6", /* Cetus */
"561313-75Dd3RRRRRRr6Uu5LLLLLLl13", /* Chamaeleon */
"341507-55Ddr2d3r3d4RrDdRr2Uu5r2uLl10Uu5Ll7", /* Circinus */
"660603-27Dd3l7Dd3Rr14UUu3Ll3", /* Columba */
"561207+33d2Ll6Dd2l3Dd6RrdRUu8l2Uu3l5", /* Coma Berenices */
"561905-37Dd5RRUu3Ll5", /* Corona Australis */
"451606+39Dd4r2dRr11Uu2l4u7Ll6", /* Corona Borealis */
"551214-12Dd3r4d2RrUu8Ll14", /* Corvus */
"551114-07DDd5RrUrUu3Ll14", /* Crater */
"561214-55Dd4RrUu5Ll14", /* Crux */
"552010+61Dd5LLlDd5RdrDd4r2Dd2RuRr4d2r6Ulu7lUu4r4u4rUu6l5u2l5ULl9ul",
/* Cygnus */
"542010+21DLl2d8RrDd4rd4r7u7r3Uu6l2Uul5", /* Delphinus */
"570408-49Dd4Lld3l7DdLd3l8DRRr6Uur3u3r5u3Rr2u2Ll2Uul6", /* Dorado */
"352013+86d5r12Dd5l9Dd3r3d5RDr3d2r5d2r5Dd2Rr10UuRr14uRRr3ur8u2Rr8Uu2Rr7uRr14u\
3Rr8Uu3RRr9UuLl13DdLl9d3LLlDdLd4Ll10ULl8u5Ll7Ul7u6LLLl13", /* Draco */
"562107+13Dd8Rr2u4lUu2Ll3ul4", /* Equuleus */
"430411+00d4Ll3DdRd3rDd7r3Dr2d7r5DRrd4r7d2r6d3Rr4Ddr4d3r4d4Rr8u5l3u2l4Uu2Ll6U\
Llul7u4l4Uu6Rr4UUu9Ll10ULl11", /* Eridanus */
"550312-24Dd5r4d4r7DRRr4Uu6LLl12", /* Fornax */
"560713+35d2Ll2Dd2RDrd7r5dr7Dru2Rr9u6rUu2Ru6l10Uu5Ll13", /* Gemini */
"442307-36DDd6Rr13UuRr8Uu4LLl7", /* Grus */
"551805+51Dd2rDDl3d4l7Dd8r8u2Rr9dRr2Dd6r10Uu6RUu2l2u3l2ul2Uu9Rr3UUu2LLldLl5",
/* Hercules */
"770404-40d9r2DdRr2d2r5d4r4Dd7Rr12Uu6l4u3l4UuLlu3l6u2l7ULl4", /* Horologium */
"760910+07DDd2Ll13Dld5LLl10u2LLl6d2LlDRRRr5d4r5d2RRru4r4Ur5u3Rr2u2r10URr5u2r3\
u7r4UUu7Ll10", /* Hydra */
"560203-58Dd7LLl9Dd5Rr8Dd2RRRr14Uu6l12dLl6UUu2Ll3", /* Hydrus */
"742107-45d4Ll2DDd7Ll7Dd4RRr8UURr8Uu5Ll7", /* Indus */
"562214+57DDd5r13uRrUu4LulUu3l3u3l3ul7", /* Lacerta */
"551200+28DDr6Dd7Rr2Uu7Rr9UUUu3LDd2l9d5l4u2Ll2u3L", /* Leo */
"451004+41Ddl9d6Ll2Dd5Rr2d2r4u5RUu3r9u6l5UuLl4", /* Leo Minor */
"550603-11Dd7RRrUu6Lu3Ll3", /* Lepus */
"551600-04DDdr3Dr11u5Rr9UUu2l6u7Ll3d3L", /* Libra */
"331602-30Dd2Rr3d6r5Dd4r4dRr11Uu7LlUULl2", /* Lupus */
"640703+62DLl9Dd4Ll5d5l6Ddr5d6RRr2u2r6Uu4RrUr4u4r6Uu2Ll3", /* Lynx */
"551903+48d4l4Dd3rDrd5Rrur7Ur3Uu8Ll3", /* Lyra */
"560608-70d5Ll9Dd5RRRRr8Uu5Ll9ULLl8", /* Mensa */
"552107-27DDd5Rr8UUu3Ll7", /* Microscopium */
"470701+12Dld9l3DLl3DdRRRrUu6Ll5UUlu2Ll", /* Monoceros */
"561311-64dl2Dd5RRr10Uu6LLl11", /* Musca */
"561609-42DDRr8u5r4ul4Uu2l5u6Ll9", /* Norma */
"270000-74lDd2LLLl7d3LLLLl9u2LLLLLLLLLLLl4Uu6LLLLLL", /* Octans */
"641806+14d2l5Dd4r5d2l2dr2DRrd4LlDRr3d2rUr7d6l7DDRr2u5r7UlurUu2Ru4l6Uu4l7Uu3L\
l6uLl6", /* Ophiuchus */
"560600+23dl5Dd2lDrDd4RrDdr11Uu6Rr4UrUu5Llul5dl4d3l3u6rUu3L", /* Orion */
"552007-57DLl7Dd4RRRr11Uu2Rr5Uu3LLLl7", /* Pavo */
"552201+36dLl9d2l4dLl2Ddld6lDd7r2d2RrDd2Rr2d6Rr3ur3dr2Uu3r4drUl3u4l3u4l4Uu6Ll\
", /* Pegasus */
"460209+59d2Ll5d2l2d2Ll12DDd4r3d5RRr2u3r2u3rUUur7Dd2Rr3Uur4u4l4u3Lul9",
/* Perseus */
"650206-40d8RrDdr4d2r3d5RRr8UULLLl6", /* Phoenix */
"640601-43Dd3l2d2l6d3l4Dd4Ru3r7Uu3r7u3Rr7Uu4l5u3LLl", /* Pictor */
"450108+33Ddl4Dd9Ll2Dd7RRr8Dd6Rru3Rr2Uu8Ll14UuLl2u2l2Uu2l10u3r2Uu3Ll8",
/* Pisces */
"562302-25Dd6RRr8Uu5LLl2", /* Piscis Austrinus */
"570807-11DDDd3r6DdRRr14Uu7l9Uu7Ll7UUu9Ll7", /* Puppis */
"560810-18DLl3d5l4Dd7Rr8UUu2l3", /* Pyxis */
"560401-53d3l5d3l3Dd7Rr11Uu3l4u4Ll", /* Reticulum */
"452005+22Dd4Rrur13u2RrUu2Ll5Dl9u2Ll5", /* Sagitta */
"552002-12Dd7l5DDd5Rr10Uu3RRUr4Uu4LLu4Ll2", /* Sagittarius */
"471606-08Dd9lDrd5l7DLLDd5Rr6u3r7UURr3u9LUUu2l6", /* Scorpius */
"560111-25DDRRr8u4r5Uu5LLl11", /* Sculptor */
"551900-04Dd6r9Uu6L", /* Scutum */
"861814+06d4r4Dd4r4Dd6Rr11Ul7d2lu2Llu6RrULl6u3l2ur2u2l8bRbRbRbUbUd4l3Dd6l3Dd4\
Rr12UUUu5Ll2d3RD", /* Serpens */
"551013+07DDd2Rr5UUu7Ll13", /* Sextans */
"640600+29d6r3Dd2ld6r3u3r4ur5dRr5DDRr5dr4UUUlUuLl9Dl4dLL", /* Taurus */
"552007-45Dd7RRr12Uu5LLl7", /* Telescopium */
"560211+37d3l2d3r5Dd2r7d2Rr3u3r4Uu5Ll2u2l9", /* Triangulum */
"561609-60dl3d3l2dLd3l3DdRRRrUu2Ll2u4l3u3l2ULl9", /* Triangulum Australe */
"360106-58DDd5Rr2uRr8Uu3Rr13Uu4Ll7d2LLl6", /* Tucana */
"641107+73Dd4Lld3Ll8dLl7Dd5r5Dd2Rr6Uu2Rr12Dd6rDDd2Rr13Uu3Rr2u6r9UuRr10u5Rr6UU\
r6Uu3LLLl7", /* Ursa Major */
"342200+86RRRRrDr7d5Rr7DRr5d5RRu4r14Uu6l8u3Ll7Uu6RRRRRRr10u2RRRRRRRRRRrd2R",
/* Ursa Minor */
"560907-37DLLlDd7RRRr2u2r5u2r4u2r3Uu7l6Uu3Ll7", /* Vela */
"551309+14Dd2LLl3DdRr3d7r6DDd3RRrUu8RrUu3r5UULu3l14uLl9", /* Virgo */
"560900-64Dd5RRr7Uu6LLL", /* Volans */
"462100+29dl8d4r3DRr5ur5uRrDr9u2Rru3Ll5u2l6u2LL"}; /* Vulpecula */
#endif /* CONSTEL */
#endif /* GRAPH */

/* xdata.c */
