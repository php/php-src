/* 
   Copyright (C) 1998, 1999 by Pruet Boonma <pruet@eng.cmu.ac.th>
   Copyright (C) 1998  by Theppitak Karoonboonyanan <thep@links.nectec.or.th>
   Copyright (C) 1989, 1991 by Samphan Raruenrom <samphan@thai.com>

   Permission to use, copy, modify, distribute and sell this software
   and its documentation for any purpose is hereby granted without fee,
   provided that the above copyright notice appear in all copies.
   Smaphan Raruenrom , Theppitak Karoonboonyanan and Pruet Boonma makes
   no representations about the suitability of this software for any 
   purpose.  It is provided  "as is" without express or implied warranty.
*/


/*
   This file is basicly tis620 character sets with some extra functions
   for tis-620 handling
*/

/*
 * This comment is parsed by configure to create ctype.c,
 * so don't change it unless you know what you are doing.
 *
 * .configure. strxfrm_multiply_tis620=4
 */

#include <global.h>
#include <my_sys.h>
#include "m_string.h"
#include "m_ctype.h"
#include "t_ctype.h"

static uchar* thai2sortable(const uchar *tstr,uint len);

#define BUFFER_MULTIPLY	4
#define buffsize(s)	(BUFFER_MULTIPLY * (strlen(s) + 1))
#define M  L_MIDDLE
#define U  L_UPPER
#define L  L_LOWER
#define UU L_UPRUPR
#define X  L_MIDDLE


int t_ctype[][TOT_LEVELS] = {
    /*0x00*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x01*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x02*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x03*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x04*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x05*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x06*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x07*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x08*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x09*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x0A*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x0B*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x0C*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x0D*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x0E*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x0F*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x10*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x11*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x12*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x13*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x14*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x15*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x16*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x17*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x18*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x19*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x1A*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x1B*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x1C*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x1D*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x1E*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x1F*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0x20*/ { IGNORE, IGNORE, L3_SPACE, IGNORE, M},  
    /*0x21*/ { IGNORE, IGNORE, L3_EXCLAMATION, IGNORE, M    }, 
    /*0x22*/ { IGNORE, IGNORE, L3_QUOTATION, IGNORE, M      }, 
    /*0x23*/ { IGNORE, IGNORE, L3_NUMBER, IGNORE, M         }, 
    /*0x24*/ { IGNORE, IGNORE, L3_DOLLAR, IGNORE, M         }, 
    /*0x25*/ { IGNORE, IGNORE, L3_PERCENT, IGNORE, M        },
    /*0x26*/ { IGNORE, IGNORE, L3_AMPERSAND, IGNORE, M      },
    /*0x27*/ { IGNORE, IGNORE, L3_APOSTROPHE, IGNORE, M     },
    /*0x28*/ { IGNORE, IGNORE, L3_L_PARANTHESIS, IGNORE, M  },
    /*0x29*/ { IGNORE, IGNORE, L3_R_PARENTHESIS, IGNORE, M  },
    /*0x2A*/ { IGNORE, IGNORE, L3_ASTERISK, IGNORE, M       },
    /*0x2B*/ { IGNORE, IGNORE, L3_PLUS, IGNORE, M           },
    /*0x2C*/ { IGNORE, IGNORE, L3_COMMA, IGNORE, M          },
    /*0x2D*/ { IGNORE, IGNORE, L3_HYPHEN, IGNORE, M         },
    /*0x2E*/ { IGNORE, IGNORE, L3_FULL_STOP, IGNORE, M      },
    /*0x2F*/ { IGNORE, IGNORE, L3_SOLIDUS, IGNORE, M        },
    /*0x30*/ { L1_08, L2_BLANK, L3_BLANK, L4_BLANK, M  },
    /*0x31*/ { L1_18, L2_BLANK, L3_BLANK, L4_BLANK, M  },
    /*0x32*/ { L1_28, L2_BLANK, L3_BLANK, L4_BLANK, M  },
    /*0x33*/ { L1_38, L2_BLANK, L3_BLANK, L4_BLANK, M  }, 
    /*0x34*/ { L1_48, L2_BLANK, L3_BLANK, L4_BLANK, M  },
    /*0x35*/ { L1_58, L2_BLANK, L3_BLANK, L4_BLANK, M  }, 
    /*0x36*/ { L1_68, L2_BLANK, L3_BLANK, L4_BLANK, M  }, 
    /*0x37*/ { L1_78, L2_BLANK, L3_BLANK, L4_BLANK, M  },
    /*0x38*/ { L1_88, L2_BLANK, L3_BLANK, L4_BLANK, M  },
    /*0x39*/ { L1_98, L2_BLANK, L3_BLANK, L4_BLANK, M  },
    /*0x3A*/ { IGNORE, IGNORE, L3_COLON, IGNORE, M          }, 
    /*0x3B*/ { IGNORE, IGNORE, L3_SEMICOLON, IGNORE, M      },
    /*0x3C*/ { IGNORE, IGNORE, L3_LESS_THAN, IGNORE, M      },
    /*0x3D*/ { IGNORE, IGNORE, L3_EQUAL, IGNORE, M          },
    /*0x3E*/ { IGNORE, IGNORE, L3_GREATER_THAN, IGNORE, M   },
    /*0x3F*/ { IGNORE, IGNORE, L3_QUESTION, IGNORE, M       },
    /*0x40*/ { IGNORE, IGNORE, L3_AT, IGNORE, M             },
    /*0x41*/ { L1_A8, L2_BLANK, L3_BLANK, L4_CAP, M  },
    /*0x42*/ { L1_B8, L2_BLANK, L3_BLANK, L4_CAP, M  },
    /*0x43*/ { L1_C8, L2_BLANK, L3_BLANK, L4_CAP, M  },
    /*0x44*/ { L1_D8, L2_BLANK, L3_BLANK, L4_CAP, M  },
    /*0x45*/ { L1_E8, L2_BLANK, L3_BLANK, L4_CAP, M  },
    /*0x46*/ { L1_F8, L2_BLANK, L3_BLANK, L4_CAP, M  },
    /*0x47*/ { L1_G8, L2_BLANK, L3_BLANK, L4_CAP, M  },
    /*0x48*/ { L1_H8, L2_BLANK, L3_BLANK, L4_CAP, M  },
    /*0x49*/ { L1_I8, L2_BLANK, L3_BLANK, L4_CAP, M  },
    /*0x4A*/ { L1_J8, L2_BLANK, L3_BLANK, L4_CAP, M  },
    /*0x4B*/ { L1_K8, L2_BLANK, L3_BLANK, L4_CAP, M  },
    /*0x4C*/ { L1_L8, L2_BLANK, L3_BLANK, L4_CAP, M  },
    /*0x4D*/ { L1_M8, L2_BLANK, L3_BLANK, L4_CAP, M  }, 
    /*0x4E*/ { L1_N8, L2_BLANK, L3_BLANK, L4_CAP, M  }, 
    /*0x4F*/ { L1_O8, L2_BLANK, L3_BLANK, L4_CAP, M  }, 
    /*0x50*/ { L1_P8, L2_BLANK, L3_BLANK, L4_CAP, M  }, 
    /*0x51*/ { L1_Q8, L2_BLANK, L3_BLANK, L4_CAP, M  }, 
    /*0x52*/ { L1_R8, L2_BLANK, L3_BLANK, L4_CAP, M  },
    /*0x53*/ { L1_S8, L2_BLANK, L3_BLANK, L4_CAP, M  },
    /*0x54*/ { L1_T8, L2_BLANK, L3_BLANK, L4_CAP, M  },
    /*0x55*/ { L1_U8, L2_BLANK, L3_BLANK, L4_CAP, M  },
    /*0x56*/ { L1_V8, L2_BLANK, L3_BLANK, L4_CAP, M  },
    /*0x57*/ { L1_W8, L2_BLANK, L3_BLANK, L4_CAP, M  },
    /*0x58*/ { L1_X8, L2_BLANK, L3_BLANK, L4_CAP, M  },
    /*0x59*/ { L1_Y8, L2_BLANK, L3_BLANK, L4_CAP, M  },
    /*0x5A*/ { L1_Z8, L2_BLANK, L3_BLANK, L4_CAP, M  },
    /*0x5B*/ { IGNORE, IGNORE, L3_L_BRACKET, IGNORE, M      },
    /*0x5C*/ { IGNORE, IGNORE, L3_BK_SOLIDUS, IGNORE, M     },
    /*0x5D*/ { IGNORE, IGNORE, L3_R_BRACKET, IGNORE, M      },
    /*0x5E*/ { IGNORE, IGNORE, L3_CIRCUMFLEX, IGNORE, M     },
    /*0x5F*/ { IGNORE, IGNORE, L3_LOW_LINE, IGNORE, M       },
    /*0x60*/ { IGNORE, IGNORE, L3_GRAVE, IGNORE, M          },
    /*0x61*/ { L1_A8, L2_BLANK, L3_BLANK, L4_MIN, M  },
    /*0x62*/ { L1_B8, L2_BLANK, L3_BLANK, L4_MIN, M  },
    /*0x63*/ { L1_C8, L2_BLANK, L3_BLANK, L4_MIN, M  },
    /*0x64*/ { L1_D8, L2_BLANK, L3_BLANK, L4_MIN, M  },
    /*0x65*/ { L1_E8, L2_BLANK, L3_BLANK, L4_MIN, M  },
    /*0x66*/ { L1_F8, L2_BLANK, L3_BLANK, L4_MIN, M  },
    /*0x67*/ { L1_G8, L2_BLANK, L3_BLANK, L4_MIN, M  },
    /*0x68*/ { L1_H8, L2_BLANK, L3_BLANK, L4_MIN, M  },
    /*0x69*/ { L1_I8, L2_BLANK, L3_BLANK, L4_MIN, M  },
    /*0x6A*/ { L1_J8, L2_BLANK, L3_BLANK, L4_MIN, M  },
    /*0x6B*/ { L1_K8, L2_BLANK, L3_BLANK, L4_MIN, M  },
    /*0x6C*/ { L1_L8, L2_BLANK, L3_BLANK, L4_MIN, M  },
    /*0x6D*/ { L1_M8, L2_BLANK, L3_BLANK, L4_MIN, M  },
    /*0x6E*/ { L1_N8, L2_BLANK, L3_BLANK, L4_MIN, M  },
    /*0x6F*/ { L1_O8, L2_BLANK, L3_BLANK, L4_MIN, M  },
    /*0x70*/ { L1_P8, L2_BLANK, L3_BLANK, L4_MIN, M  },
    /*0x71*/ { L1_Q8, L2_BLANK, L3_BLANK, L4_MIN, M  },
    /*0x72*/ { L1_R8, L2_BLANK, L3_BLANK, L4_MIN, M  },
    /*0x73*/ { L1_S8, L2_BLANK, L3_BLANK, L4_MIN, M  },
    /*0x74*/ { L1_T8, L2_BLANK, L3_BLANK, L4_MIN, M  },
    /*0x75*/ { L1_U8, L2_BLANK, L3_BLANK, L4_MIN, M  },
    /*0x76*/ { L1_V8, L2_BLANK, L3_BLANK, L4_MIN, M  },
    /*0x77*/ { L1_W8, L2_BLANK, L3_BLANK, L4_MIN, M  },
    /*0x78*/ { L1_X8, L2_BLANK, L3_BLANK, L4_MIN, M  },
    /*0x79*/ { L1_Y8, L2_BLANK, L3_BLANK, L4_MIN, M  },
    /*0x7A*/ { L1_Z8, L2_BLANK, L3_BLANK, L4_MIN, M  },
    /*0x7B*/ { IGNORE, IGNORE, L3_L_BRACE, IGNORE, M        },
    /*0x7C*/ { IGNORE, IGNORE, L3_V_LINE, IGNORE, M         },
    /*0x7D*/ { IGNORE, IGNORE, L3_R_BRACE, IGNORE, M        },
    /*0x7E*/ { IGNORE, IGNORE, L3_TILDE, IGNORE, M          },
    /*0x7F*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x80*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x81*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x82*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x83*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x84*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x85*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x86*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x87*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x88*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x89*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x8A*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x8B*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x8C*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x8D*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x8E*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x8F*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x90*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x91*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x92*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x93*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x94*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x95*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x96*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x97*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x98*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x99*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x9A*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x9B*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x9C*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x9D*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x9E*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0x9F*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0xA0*/ { IGNORE, IGNORE, L3_NB_SACE, IGNORE, X        },
    /*0xA1*/ { L1_KO_KAI, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xA2*/ { L1_KHO_KHAI, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt}, 
    /*0xA3*/ { L1_KHO_KHUAT, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xA4*/ { L1_KHO_KHWAI, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xA5*/ { L1_KHO_KHON, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xA6*/ { L1_KHO_RAKHANG, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xA7*/ { L1_NGO_NGU, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt}, 
    /*0xA8*/ { L1_CHO_CHAN, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xA9*/ { L1_CHO_CHING, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xAA*/ { L1_CHO_CHANG, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xAB*/ { L1_SO_SO, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt}, 
    /*0xAC*/ { L1_CHO_CHOE,	L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xAD*/ { L1_YO_YING, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt}, 
    /*0xAE*/ { L1_DO_CHADA,	L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xAF*/ { L1_TO_PATAK,	L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xB0*/ { L1_THO_THAN,	L2_BLANK,L3_BLANK, L4_BLANK, M | _consnt}, 
    /*0xB1*/ { L1_THO_NANGMONTHO, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt}, 
    /*0xB2*/ { L1_THO_PHUTHAO, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xB3*/ { L1_NO_NEN, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xB4*/ { L1_DO_DEK, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xB5*/ { L1_TO_TAO, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt}, 
    /*0xB6*/ { L1_THO_THUNG, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xB7*/ { L1_THO_THAHAN, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xB8*/ { L1_THO_THONG, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt}, 
    /*0xB9*/ { L1_NO_NU, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xBA*/ { L1_BO_BAIMAI, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xBB*/ { L1_PO_PLA, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xBC*/ { L1_PHO_PHUNG, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xBD*/ { L1_FO_FA, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xBE*/ { L1_PHO_PHAN, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xBF*/ { L1_FO_FAN, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xC0*/ { L1_PHO_SAMPHAO, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xC1*/ { L1_MO_MA, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xC2*/ { L1_YO_YAK, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xC3*/ { L1_RO_RUA, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xC4*/ { L1_RU, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt}, 
    /*0xC5*/ { L1_LO_LING, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xC6*/ { L1_LU, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt}, 
    /*0xC7*/ { L1_WO_WAEN, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt}, 
    /*0xC8*/ { L1_SO_SALA, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt}, 
    /*0xC9*/ { L1_SO_RUSI, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt}, 
    /*0xCA*/ { L1_SO_SUA, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt}, 
    /*0xCB*/ { L1_HO_HIP, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt}, 
    /*0xCC*/ { L1_LO_CHULA,	L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xCD*/ { L1_O_ANG, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt},
    /*0xCE*/ { L1_HO_NOKHUK, L2_BLANK, L3_BLANK, L4_BLANK, M | _consnt}, 
    /*0xCF*/ { IGNORE, IGNORE, L3_PAIYAN_NOI, IGNORE, M}, 
    /*0xD0*/ { L1_SARA_A, L2_BLANK, L3_BLANK, L4_BLANK, M | _fllwvowel}, 
    /*0xD1*/ { L1_MAI_HAN_AKAT, L2_BLANK, L3_BLANK, L4_BLANK, U | _uprvowel}, 
    /*0xD2*/ { L1_SARA_AA, L2_BLANK, L3_BLANK, L4_BLANK, M | _fllwvowel}, 
    /*0xD3*/ { L1_SARA_AM, L2_BLANK, L3_BLANK, L4_BLANK, M | _fllwvowel},
    /*0xD4*/ { L1_SARA_I, L2_BLANK, L3_BLANK, L4_BLANK, U | _uprvowel},
    /*0xD5*/ { L1_SARA_II, L2_BLANK, L3_BLANK, L4_BLANK, U | _uprvowel}, 
    /*0xD6*/ { L1_SARA_UE, L2_BLANK, L3_BLANK, L4_BLANK, U | _uprvowel}, 
    /*0xD7*/ { L1_SARA_UEE, L2_BLANK, L3_BLANK, L4_BLANK, U | _uprvowel},
    /*0xD8*/ { L1_SARA_U, L2_BLANK, L3_BLANK, L4_BLANK, L | _lwrvowel}, 
    /*0xD9*/ { L1_SARA_UU, L2_BLANK, L3_BLANK, L4_BLANK, L | _lwrvowel},
    /*0xDA*/ { IGNORE, L2_PINTHU, L3_BLANK, L4_BLANK, L  },
    /*0xDB*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0xDC*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0xDD*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0xDE*/ { IGNORE, IGNORE, IGNORE, IGNORE, X }, 
    /*0xDF*/ { IGNORE, IGNORE, L3_BAHT, IGNORE, M},
    /*0xE0*/ { L1_SARA_E, L2_BLANK, L3_BLANK, L4_BLANK, M | _ldvowel },
    /*0xE1*/ { L1_SARA_AE, L2_BLANK, L3_BLANK, L4_BLANK, M | _ldvowel },
    /*0xE2*/ { L1_SARA_O, L2_BLANK, L3_BLANK, L4_BLANK, M | _ldvowel  },
    /*0xE3*/ { L1_SARA_AI_MAIMUAN, 	L2_BLANK, L3_BLANK, L4_BLANK, M | _ldvowel }, 
    /*0xE4*/ { L1_SARA_AI_MAIMALAI, L2_BLANK, L3_BLANK, L4_BLANK, M | _ldvowel },
    /*0xE5*/ { L1_SARA_AA, L2_BLANK, L3_BLANK, L4_EXT, M  | _fllwvowel }, 
    /*0xE6*/ { IGNORE, IGNORE, L3_MAI_YAMOK, IGNORE, M | _stone },
    /*0xE7*/ { IGNORE, L2_TYKHU, L3_BLANK, L4_BLANK, U | _diacrt1 | _stone  },
    /*0xE8*/ { IGNORE, L2_TONE1, L3_BLANK, L4_BLANK, UU | _tone | _combine | _stone  },
    /*0xE9*/ { IGNORE, L2_TONE2, L3_BLANK, L4_BLANK, UU | _tone | _combine | _stone  },
    /*0xEA*/ { IGNORE, L2_TONE3, L3_BLANK, L4_BLANK, UU | _tone | _combine | _stone  },
    /*0xEB*/ { IGNORE, L2_TONE4, L3_BLANK, L4_BLANK, UU | _tone | _combine | _stone },
    /*0xEC*/ { IGNORE, L2_GARAN, L3_BLANK, L4_BLANK, UU | _diacrt2 | _combine | _stone },
    /*0xED*/ { L1_NKHIT, L2_BLANK, L3_BLANK, L4_BLANK, U | _diacrt1  },
    /*0xEE*/ { IGNORE, L2_YAMAK, L3_BLANK, L4_BLANK, U | _diacrt1 },
    /*0xEF*/ { IGNORE, IGNORE, L3_FONGMAN, IGNORE, M  }, 
    /*0xF0*/ { L1_08, L2_THAII, L3_BLANK, L4_BLANK, M | _tdig }, 
    /*0xF1*/ { L1_18, L2_THAII, L3_BLANK, L4_BLANK, M | _tdig },
    /*0xF2*/ { L1_28, L2_THAII, L3_BLANK, L4_BLANK, M | _tdig },
    /*0xF3*/ { L1_38, L2_THAII, L3_BLANK, L4_BLANK, M | _tdig },
    /*0xF4*/ { L1_48, L2_THAII, L3_BLANK, L4_BLANK, M | _tdig },
    /*0xF5*/ { L1_58, L2_THAII, L3_BLANK, L4_BLANK, M | _tdig },
    /*0xF6*/ { L1_68, L2_THAII, L3_BLANK, L4_BLANK, M | _tdig },
    /*0xF7*/ { L1_78, L2_THAII, L3_BLANK, L4_BLANK, M | _tdig },
    /*0xF8*/ { L1_88, L2_THAII, L3_BLANK, L4_BLANK, M | _tdig },
    /*0xF9*/ { L1_98, L2_THAII, L3_BLANK, L4_BLANK, M | _tdig },
    /*0xFA*/ { IGNORE, IGNORE, L3_ANGKHANKHU, IGNORE, X },
    /*0xFB*/ { IGNORE, IGNORE, L3_KHOMUT, IGNORE, X },
    /*0xFC*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0xFD*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0xFE*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
    /*0xFF*/ { IGNORE, IGNORE, IGNORE, IGNORE, X },
};

uchar NEAR ctype_tis620[257] =
{
  0,				/* For standard library */
  32,32,32,32,32,32,32,32,32,40,40,40,40,40,32,32,
  32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
  72,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
  132,132,132,132,132,132,132,132,132,132,16,16,16,16,16,16,
  16,129,129,129,129,129,129,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,16,16,16,16,16,
  16,130,130,130,130,130,130,2,2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,2,2,2,16,16,16,16,32,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

uchar NEAR to_lower_tis620[]=
{
  '\000','\001','\002','\003','\004','\005','\006','\007',
  '\010','\011','\012','\013','\014','\015','\016','\017',
  '\020','\021','\022','\023','\024','\025','\026','\027',
  '\030','\031','\032','\033','\034','\035','\036','\037',
  ' ',	 '!',	'"',   '#',   '$',   '%',   '&',   '\'',
  '(',	 ')',	'*',   '+',   ',',   '-',   '.',   '/',
  '0',	 '1',	'2',   '3',   '4',   '5',   '6',   '7',
  '8',	 '9',	':',   ';',   '<',   '=',   '>',   '?',
  '@',	 'a',	'b',   'c',   'd',   'e',   'f',   'g',
  'h',	 'i',	'j',   'k',   'l',   'm',   'n',   'o',
  'p',	 'q',	'r',   's',   't',   'u',   'v',   'w',
  'x',	 'y',	'z',   '[',   '\\',  ']',   '^',   '_',
  '`',	 'a',	'b',   'c',   'd',   'e',   'f',   'g',
  'h',	 'i',	'j',   'k',   'l',   'm',   'n',   'o',
  'p',	 'q',	'r',   's',   't',   'u',   'v',   'w',
  'x',	 'y',	'z',   '{',   '|',   '}',   '~',   '\177',
  (uchar) '\200',(uchar) '\201',(uchar) '\202',(uchar) '\203',(uchar) '\204',(uchar) '\205',(uchar) '\206',(uchar) '\207',
  (uchar) '\210',(uchar) '\211',(uchar) '\212',(uchar) '\213',(uchar) '\214',(uchar) '\215',(uchar) '\216',(uchar) '\217',
  (uchar) '\220',(uchar) '\221',(uchar) '\222',(uchar) '\223',(uchar) '\224',(uchar) '\225',(uchar) '\226',(uchar) '\227',
  (uchar) '\230',(uchar) '\231',(uchar) '\232',(uchar) '\233',(uchar) '\234',(uchar) '\235',(uchar) '\236',(uchar) '\237',
  (uchar) '\240',(uchar) '\241',(uchar) '\242',(uchar) '\243',(uchar) '\244',(uchar) '\245',(uchar) '\246',(uchar) '\247',
  (uchar) '\250',(uchar) '\251',(uchar) '\252',(uchar) '\253',(uchar) '\254',(uchar) '\255',(uchar) '\256',(uchar) '\257',
  (uchar) '\260',(uchar) '\261',(uchar) '\262',(uchar) '\263',(uchar) '\264',(uchar) '\265',(uchar) '\266',(uchar) '\267',
  (uchar) '\270',(uchar) '\271',(uchar) '\272',(uchar) '\273',(uchar) '\274',(uchar) '\275',(uchar) '\276',(uchar) '\277',
  (uchar) '\300',(uchar) '\301',(uchar) '\302',(uchar) '\303',(uchar) '\304',(uchar) '\305',(uchar) '\306',(uchar) '\307',
  (uchar) '\310',(uchar) '\311',(uchar) '\312',(uchar) '\313',(uchar) '\314',(uchar) '\315',(uchar) '\316',(uchar) '\317',
  (uchar) '\320',(uchar) '\321',(uchar) '\322',(uchar) '\323',(uchar) '\324',(uchar) '\325',(uchar) '\326',(uchar) '\327',
  (uchar) '\330',(uchar) '\331',(uchar) '\332',(uchar) '\333',(uchar) '\334',(uchar) '\335',(uchar) '\336',(uchar) '\337',
  (uchar) '\340',(uchar) '\341',(uchar) '\342',(uchar) '\343',(uchar) '\344',(uchar) '\345',(uchar) '\346',(uchar) '\347',
  (uchar) '\350',(uchar) '\351',(uchar) '\352',(uchar) '\353',(uchar) '\354',(uchar) '\355',(uchar) '\356',(uchar) '\357',
  (uchar) '\360',(uchar) '\361',(uchar) '\362',(uchar) '\363',(uchar) '\364',(uchar) '\365',(uchar) '\366',(uchar) '\367',
  (uchar) '\370',(uchar) '\371',(uchar) '\372',(uchar) '\373',(uchar) '\374',(uchar) '\375',(uchar) '\376',(uchar) '\377',
};

uchar NEAR to_upper_tis620[]=
{
  '\000','\001','\002','\003','\004','\005','\006','\007',
  '\010','\011','\012','\013','\014','\015','\016','\017',
  '\020','\021','\022','\023','\024','\025','\026','\027',
  '\030','\031','\032','\033','\034','\035','\036','\037',
  ' ',	 '!',	'"',   '#',   '$',   '%',   '&',   '\'',
  '(',	 ')',	'*',   '+',   ',',   '-',   '.',   '/',
  '0',	 '1',	'2',   '3',   '4',   '5',   '6',   '7',
  '8',	 '9',	':',   ';',   '<',   '=',   '>',   '?',
  '@',	 'A',	'B',   'C',   'D',   'E',   'F',   'G',
  'H',	 'I',	'J',   'K',   'L',   'M',   'N',   'O',
  'P',	 'Q',	'R',   'S',   'T',   'U',   'V',   'W',
  'X',	 'Y',	'Z',   '[',   '\\',  ']',   '^',   '_',
  '`',	 'A',	'B',   'C',   'D',   'E',   'F',   'G',
  'H',	 'I',	'J',   'K',   'L',   'M',   'N',   'O',
  'P',	 'Q',	'R',   'S',   'T',   'U',   'V',   'W',
  'X',	 'Y',	'Z',   '{',   '|',   '}',   '~',   '\177',
  (uchar) '\200',(uchar) '\201',(uchar) '\202',(uchar) '\203',(uchar) '\204',(uchar) '\205',(uchar) '\206',(uchar) '\207',
  (uchar) '\210',(uchar) '\211',(uchar) '\212',(uchar) '\213',(uchar) '\214',(uchar) '\215',(uchar) '\216',(uchar) '\217',
  (uchar) '\220',(uchar) '\221',(uchar) '\222',(uchar) '\223',(uchar) '\224',(uchar) '\225',(uchar) '\226',(uchar) '\227',
  (uchar) '\230',(uchar) '\231',(uchar) '\232',(uchar) '\233',(uchar) '\234',(uchar) '\235',(uchar) '\236',(uchar) '\237',
  (uchar) '\240',(uchar) '\241',(uchar) '\242',(uchar) '\243',(uchar) '\244',(uchar) '\245',(uchar) '\246',(uchar) '\247',
  (uchar) '\250',(uchar) '\251',(uchar) '\252',(uchar) '\253',(uchar) '\254',(uchar) '\255',(uchar) '\256',(uchar) '\257',
  (uchar) '\260',(uchar) '\261',(uchar) '\262',(uchar) '\263',(uchar) '\264',(uchar) '\265',(uchar) '\266',(uchar) '\267',
  (uchar) '\270',(uchar) '\271',(uchar) '\272',(uchar) '\273',(uchar) '\274',(uchar) '\275',(uchar) '\276',(uchar) '\277',
  (uchar) '\300',(uchar) '\301',(uchar) '\302',(uchar) '\303',(uchar) '\304',(uchar) '\305',(uchar) '\306',(uchar) '\307',
  (uchar) '\310',(uchar) '\311',(uchar) '\312',(uchar) '\313',(uchar) '\314',(uchar) '\315',(uchar) '\316',(uchar) '\317',
  (uchar) '\320',(uchar) '\321',(uchar) '\322',(uchar) '\323',(uchar) '\324',(uchar) '\325',(uchar) '\326',(uchar) '\327',
  (uchar) '\330',(uchar) '\331',(uchar) '\332',(uchar) '\333',(uchar) '\334',(uchar) '\335',(uchar) '\336',(uchar) '\337',
  (uchar) '\340',(uchar) '\341',(uchar) '\342',(uchar) '\343',(uchar) '\344',(uchar) '\345',(uchar) '\346',(uchar) '\347',
  (uchar) '\350',(uchar) '\351',(uchar) '\352',(uchar) '\353',(uchar) '\354',(uchar) '\355',(uchar) '\356',(uchar) '\357',
  (uchar) '\360',(uchar) '\361',(uchar) '\362',(uchar) '\363',(uchar) '\364',(uchar) '\365',(uchar) '\366',(uchar) '\367',
  (uchar) '\370',(uchar) '\371',(uchar) '\372',(uchar) '\373',(uchar) '\374',(uchar) '\375',(uchar) '\376',(uchar) '\377',
};

uchar NEAR sort_order_tis620[]=
{
  '\000','\001','\002','\003','\004','\005','\006','\007',
  '\010','\011','\012','\013','\014','\015','\016','\017',
  '\020','\021','\022','\023','\024','\025','\026','\027',
  '\030','\031','\032','\033','\034','\035','\036','\037',
  ' ',	 '!',	'"',   '#',   '$',   '%',   '&',   '\'',
  '(',	 ')',	'*',   '+',   ',',   '-',   '.',   '/',
  '0',	 '1',	'2',   '3',   '4',   '5',   '6',   '7',
  '8',	 '9',	':',   ';',   '<',   '=',   '>',   '?',
  '@',	 'A',	'B',   'C',   'D',   'E',   'F',   'G',
  'H',	 'I',	'J',   'K',   'L',   'M',   'N',   'O',
  'P',	 'Q',	'R',   'S',   'T',   'U',   'V',   'W',
  'X',	 'Y',	'Z',   '\\',  ']',   '[',   '^',   '_',
  'E',	 'A',	'B',   'C',   'D',   'E',   'F',   'G',
  'H',	 'I',	'J',   'K',   'L',   'M',   'N',   'O',
  'P',	 'Q',	'R',   'S',   'T',   'U',   'V',   'W',
  'X',	 'Y',	'Z',   '{',   '|',   '}',   'Y',   '\177',
  (uchar) '\200',(uchar) '\201',(uchar) '\202',(uchar) '\203',(uchar) '\204',(uchar) '\205',(uchar) '\206',(uchar) '\207',
  (uchar) '\210',(uchar) '\211',(uchar) '\212',(uchar) '\213',(uchar) '\214',(uchar) '\215',(uchar) '\216',(uchar) '\217',
  (uchar) '\220',(uchar) '\221',(uchar) '\222',(uchar) '\223',(uchar) '\224',(uchar) '\225',(uchar) '\226',(uchar) '\227',
  (uchar) '\230',(uchar) '\231',(uchar) '\232',(uchar) '\233',(uchar) '\234',(uchar) '\235',(uchar) '\236',(uchar) '\237',
  (uchar) '\240',(uchar) '\241',(uchar) '\242',(uchar) '\243',(uchar) '\244',(uchar) '\245',(uchar) '\246',(uchar) '\247',
  (uchar) '\250',(uchar) '\251',(uchar) '\252',(uchar) '\253',(uchar) '\254',(uchar) '\255',(uchar) '\256',(uchar) '\257',
  (uchar) '\260',(uchar) '\261',(uchar) '\262',(uchar) '\263',(uchar) '\264',(uchar) '\265',(uchar) '\266',(uchar) '\267',
  (uchar) '\270',(uchar) '\271',(uchar) '\272',(uchar) '\273',(uchar) '\274',(uchar) '\275',(uchar) '\276',(uchar) '\277',
  (uchar) '\300',(uchar) '\301',(uchar) '\302',(uchar) '\303',(uchar) '\304',(uchar) '\305',(uchar) '\306',(uchar) '\307',
  (uchar) '\310',(uchar) '\311',(uchar) '\312',(uchar) '\313',(uchar) '\314',(uchar) '\315',(uchar) '\316',(uchar) '\317',
  (uchar) '\320',(uchar) '\321',(uchar) '\322',(uchar) '\323',(uchar) '\324',(uchar) '\325',(uchar) '\326',(uchar) '\327',
  (uchar) '\330',(uchar) '\331',(uchar) '\332',(uchar) '\333',(uchar) '\334',(uchar) '\335',(uchar) '\336',(uchar) '\337',
  (uchar) '\340',(uchar) '\341',(uchar) '\342',(uchar) '\343',(uchar) '\344',(uchar) '\345',(uchar) '\346',(uchar) '\347',
  (uchar) '\350',(uchar) '\351',(uchar) '\352',(uchar) '\353',(uchar) '\354',(uchar) '\355',(uchar) '\356',(uchar) '\357',
  (uchar) '\360',(uchar) '\361',(uchar) '\362',(uchar) '\363',(uchar) '\364',(uchar) '\365',(uchar) '\366',(uchar) '\367',
  (uchar) '\370',(uchar) '\371',(uchar) '\372',(uchar) '\373',(uchar) '\374',(uchar) '\375',(uchar) '\376',(uchar) '\377',
};

/* Convert thai string to "Standard C String Function" sortable string 
   Arg: const source string and length of converted string
   Ret: Sortable string
*/

static uchar* thai2sortable(const uchar * tstr,uint len) 
{
  const uchar* p = tstr;
  uchar 	*outBuf;
  uchar 	*pRight1, *pRight2, *pRight3, *pRight4;
  uchar 	*pLeft1, *pLeft2, *pLeft3, *pLeft4;
  uint	bufSize;

  len = (uint) strnlen((char*) tstr,len);
  bufSize = (uint) buffsize((char*) tstr);
  if(!(pRight1 = (uchar *)malloc(sizeof(uchar) * bufSize))) {
    return( (uchar*) tstr);
  }
  pLeft1 = pRight1;
  outBuf = pRight1;
  if(!(pRight2 = (uchar *)malloc(sizeof(uchar) * (len + 1)))) {
    free(pRight1);
    return((uchar*) tstr);
  }
  pLeft2 = pRight2;
  if(!(pRight3 = (uchar *)malloc(sizeof(uchar) * (len + 1)))) {
    free(pRight1);
    free(pRight2);
    return((uchar*) tstr);
  }
  pLeft3 = pRight3;
  if(!(pRight4 = (uchar *)malloc(sizeof(uchar) * (len + 1)))) {
    free(pRight1);
    free(pRight2);
    free(pRight3);
    return((uchar*) tstr);
  }
  pLeft4 = pRight4;
  while(len--) {
    if(isldvowel(*p) && isconsnt(p[1])) {
      *pRight1++ = t_ctype[p[1]][0];
      *pRight2++ = t_ctype[p[1]][1];
      *pRight3++ = t_ctype[p[1]][2];
      *pRight4++ = t_ctype[p[1]][3];
      *pRight1++ = t_ctype[*p][0];
      *pRight2++ = t_ctype[*p][1];
      *pRight3++ = t_ctype[*p][2];
      *pRight4++ = t_ctype[*p][3];
      len--;
      p += 2;
    } else {
      *pRight1 = t_ctype[*p][0];	
      if(*pRight1 != IGNORE) pRight1++;
      *pRight2 = t_ctype[*p][1];
      if(*pRight2 != IGNORE) pRight2++;
      *pRight3 = t_ctype[*p][2];
      if(*pRight3 != IGNORE) pRight3++;
      *pRight4 = t_ctype[*p][3];
      if(*pRight4 != IGNORE) pRight4++;
      p++;
    }
  }
  *pRight1++ = L2_BLANK;
  *pRight2++ = L3_BLANK;
  *pRight3++ = L4_BLANK;
  *pRight4++ = '\0';
  memcpy(pRight1, pLeft2, pRight2 - pLeft2);
  pRight1 += pRight2 - pLeft2;
  memcpy(pRight1, pLeft3, pRight3 - pLeft3);
  pRight1 += pRight3 - pLeft3;
  memcpy(pRight1, pLeft4, pRight4 - pLeft4);
  free(pLeft2);
  free(pLeft3);
  free(pLeft4);
  return(outBuf);
}

/* strncoll() replacement, compare 2 string, both are conveted to sortable string
   Arg: 2 Strings and it compare length
   Ret: strcmp result
*/
int my_strnncoll_tis620(const uchar * s1, int len1, const uchar * s2, int len2) 
{
  uchar *tc1, *tc2;
  int i;
  tc1 = thai2sortable(s1, len1);
  tc2 = thai2sortable(s2, len2);
  i = strcmp((char*)tc1, (char*)tc2);
  free(tc1);
  free(tc2);
  return(i);
}

/* strnxfrm replacment, convert Thai string to sortable string
   Arg: Destination buffer, source string, dest length and source length
   Ret: Conveted string size
*/
int my_strnxfrm_tis620(uchar * dest, uchar * src, int len, int srclen) 
{
  uint bufSize;
  uchar *tmp;
  bufSize = (uint) buffsize((char*)src);
  tmp = thai2sortable(src,srclen);
  set_if_smaller(bufSize,(uint) len);
  memcpy((uchar *)dest, tmp, bufSize);
  free(tmp);
  return (int) bufSize;
}

/* strcoll replacment, compare 2 strings
   Arg: 2 strings
   Ret: strcmp result
*/
int my_strcoll_tis620(const uchar * s1, const uchar * s2) 
{
  uchar *tc1, *tc2;
  int i;
  tc1 = thai2sortable(s1, (uint) strlen((char*)s1));
  tc2 = thai2sortable(s2, (uint) strlen((char*)s2));
  i = strcmp((char*)tc1, (char*)tc2);
  free(tc1);
  free(tc2);
  return(i);
}

/* strxfrm replacment, convert Thai string to sortable string
   Arg: Destination buffer, String and  dest buffer size
   Ret: Converting string size
*/
int my_strxfrm_tis620(uchar * dest, uchar * src, int len)
{
  uint bufSize;
  uchar *tmp;

  bufSize = (uint) buffsize((char*) src);
  tmp = thai2sortable(src, len);
  memcpy((uchar *) dest, tmp, bufSize);
  free(tmp);
  return bufSize;
}

/* Convert SQL like string to C string
   Arg: String, its length, escape character, resource length, minimal string and maximum string
   Ret: Alway 0
*/
my_bool my_like_range_tis620(const char *ptr, uint ptr_length, pchar escape,
		             uint res_length, char *min_str, char *max_str,
		             uint *min_length,uint *max_length)
{
  char *end;
  char *min_org= min_str;
  char *min_end = min_str + res_length;
  char *tbuff;
  uchar *tc;
  uint tbuff_length;

  tbuff = (char*) (tc=thai2sortable((uchar*) ptr, ptr_length));
  tbuff_length = (uint) buffsize(ptr);
  end = tbuff + tbuff_length;
  for(;tbuff != end && min_str != min_end; tbuff++)
  {
    if(*tbuff == escape && tbuff + 1 != end)
    {
      tbuff++;
      *min_str++ = *max_str++ = *tbuff;
      continue;
    }
    if(*tbuff == '_')
    {
      *min_str++ = '\0';
      *max_str++ = '\255';
      continue;
    }
    if(*tbuff == '%')
    {
      *min_length= (uint) (min_str - min_org);
      *max_length= res_length;
      do
      {
	*min_str++ = ' ';
	*max_str++ = '\255';
      } while(min_str != min_end);
      free(tc);
      return(0);
    }
    *min_str++ = *max_str++ = *tbuff;
  }
  *min_length= *max_length = (uint) (min_str - min_org);
  while(min_str != min_end)
  {
    *min_str++ = *max_str++ = ' ';
  }
  free(tc);
  return(0);
}

/* Thai normalization for input sub system
   Arg: Buffer, 's length, String, 'length
   Ret: Void
*/
void ThNormalize(uchar* ptr, uint field_length, const uchar* from, uint length)
{
  const uchar* fr = from;
  uchar* p = ptr;

  if(length > field_length) {
    length = field_length;	
  }
  while (length--)
  {
    if((istone(*fr) || isdiacrt1(*fr)) &&
       (islwrvowel(fr[1]) || isuprvowel(fr[1])))
    {
      *p = fr[1];
      p[1] = *fr;
      fr += 2;
      p += 2;
      length--;
    }
    else
    {
      *p++ = *fr++;
    }
  }
}
