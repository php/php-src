--TEST--
html_entity_decode() conformance check (HTML 5)
--FILE--
<?php
$map = array(
	"&AElig;",
	"&AMP;",
	"&Aacute;",
	"&Abreve;",
	"&Acirc;",
	"&Acy;",
	"&Afr;",
	"&Agrave;",
	"&Alpha;",
	"&Amacr;",
	"&And;",
	"&Aogon;",
	"&Aopf;",
	"&ApplyFunction;",
	"&Aring;",
	"&Ascr;",
	"&Assign;",
	"&Atilde;",
	"&Auml;",
	"&Backslash;",
	"&Barv;",
	"&Barwed;",
	"&Bcy;",
	"&Because;",
	"&Bernoullis;",
	"&Beta;",
	"&Bfr;",
	"&Bopf;",
	"&Breve;",
	"&Bscr;",
	"&Bumpeq;",
	"&CHcy;",
	"&COPY;",
	"&Cacute;",
	"&Cap;",
	"&CapitalDifferentialD;",
	"&Cayleys;",
	"&Ccaron;",
	"&Ccedil;",
	"&Ccirc;",
	"&Cconint;",
	"&Cdot;",
	"&Cedilla;",
	"&CenterDot;",
	"&Cfr;",
	"&Chi;",
	"&CircleDot;",
	"&CircleMinus;",
	"&CirclePlus;",
	"&CircleTimes;",
	"&ClockwiseContourIntegral;",
	"&CloseCurlyDoubleQuote;",
	"&CloseCurlyQuote;",
	"&Colon;",
	"&Colone;",
	"&Congruent;",
	"&Conint;",
	"&ContourIntegral;",
	"&Copf;",
	"&Coproduct;",
	"&CounterClockwiseContourIntegral;",
	"&Cross;",
	"&Cscr;",
	"&Cup;",
	"&CupCap;",
	"&DD;",
	"&DDotrahd;",
	"&DJcy;",
	"&DScy;",
	"&DZcy;",
	"&Dagger;",
	"&Darr;",
	"&Dashv;",
	"&Dcaron;",
	"&Dcy;",
	"&Del;",
	"&Delta;",
	"&Dfr;",
	"&DiacriticalAcute;",
	"&DiacriticalDot;",
	"&DiacriticalDoubleAcute;",
	"&DiacriticalGrave;",
	"&DiacriticalTilde;",
	"&Diamond;",
	"&DifferentialD;",
	"&Dopf;",
	"&Dot;",
	"&DotDot;",
	"&DotEqual;",
	"&DoubleContourIntegral;",
	"&DoubleDot;",
	"&DoubleDownArrow;",
	"&DoubleLeftArrow;",
	"&DoubleLeftRightArrow;",
	"&DoubleLeftTee;",
	"&DoubleLongLeftArrow;",
	"&DoubleLongLeftRightArrow;",
	"&DoubleLongRightArrow;",
	"&DoubleRightArrow;",
	"&DoubleRightTee;",
	"&DoubleUpArrow;",
	"&DoubleUpDownArrow;",
	"&DoubleVerticalBar;",
	"&DownArrow;",
	"&DownArrowBar;",
	"&DownArrowUpArrow;",
	"&DownBreve;",
	"&DownLeftRightVector;",
	"&DownLeftTeeVector;",
	"&DownLeftVector;",
	"&DownLeftVectorBar;",
	"&DownRightTeeVector;",
	"&DownRightVector;",
	"&DownRightVectorBar;",
	"&DownTee;",
	"&DownTeeArrow;",
	"&Downarrow;",
	"&Dscr;",
	"&Dstrok;",
	"&ENG;",
	"&ETH;",
	"&Eacute;",
	"&Ecaron;",
	"&Ecirc;",
	"&Ecy;",
	"&Edot;",
	"&Efr;",
	"&Egrave;",
	"&Element;",
	"&Emacr;",
	"&EmptySmallSquare;",
	"&EmptyVerySmallSquare;",
	"&Eogon;",
	"&Eopf;",
	"&Epsilon;",
	"&Equal;",
	"&EqualTilde;",
	"&Equilibrium;",
	"&Escr;",
	"&Esim;",
	"&Eta;",
	"&Euml;",
	"&Exists;",
	"&ExponentialE;",
	"&Fcy;",
	"&Ffr;",
	"&FilledSmallSquare;",
	"&FilledVerySmallSquare;",
	"&Fopf;",
	"&ForAll;",
	"&Fouriertrf;",
	"&Fscr;",
	"&GJcy;",
	"&GT;",
	"&Gamma;",
	"&Gammad;",
	"&Gbreve;",
	"&Gcedil;",
	"&Gcirc;",
	"&Gcy;",
	"&Gdot;",
	"&Gfr;",
	"&Gg;",
	"&Gopf;",
	"&GreaterEqual;",
	"&GreaterEqualLess;",
	"&GreaterFullEqual;",
	"&GreaterGreater;",
	"&GreaterLess;",
	"&GreaterSlantEqual;",
	"&GreaterTilde;",
	"&Gscr;",
	"&Gt;",
	"&HARDcy;",
	"&Hacek;",
	"&Hat;",
	"&Hcirc;",
	"&Hfr;",
	"&HilbertSpace;",
	"&Hopf;",
	"&HorizontalLine;",
	"&Hscr;",
	"&Hstrok;",
	"&HumpDownHump;",
	"&HumpEqual;",
	"&IEcy;",
	"&IJlig;",
	"&IOcy;",
	"&Iacute;",
	"&Icirc;",
	"&Icy;",
	"&Idot;",
	"&Ifr;",
	"&Igrave;",
	"&Im;",
	"&Imacr;",
	"&ImaginaryI;",
	"&Implies;",
	"&Int;",
	"&Integral;",
	"&Intersection;",
	"&InvisibleComma;",
	"&InvisibleTimes;",
	"&Iogon;",
	"&Iopf;",
	"&Iota;",
	"&Iscr;",
	"&Itilde;",
	"&Iukcy;",
	"&Iuml;",
	"&Jcirc;",
	"&Jcy;",
	"&Jfr;",
	"&Jopf;",
	"&Jscr;",
	"&Jsercy;",
	"&Jukcy;",
	"&KHcy;",
	"&KJcy;",
	"&Kappa;",
	"&Kcedil;",
	"&Kcy;",
	"&Kfr;",
	"&Kopf;",
	"&Kscr;",
	"&LJcy;",
	"&LT;",
	"&Lacute;",
	"&Lambda;",
	"&Lang;",
	"&Laplacetrf;",
	"&Larr;",
	"&Lcaron;",
	"&Lcedil;",
	"&Lcy;",
	"&LeftAngleBracket;",
	"&LeftArrow;",
	"&LeftArrowBar;",
	"&LeftArrowRightArrow;",
	"&LeftCeiling;",
	"&LeftDoubleBracket;",
	"&LeftDownTeeVector;",
	"&LeftDownVector;",
	"&LeftDownVectorBar;",
	"&LeftFloor;",
	"&LeftRightArrow;",
	"&LeftRightVector;",
	"&LeftTee;",
	"&LeftTeeArrow;",
	"&LeftTeeVector;",
	"&LeftTriangle;",
	"&LeftTriangleBar;",
	"&LeftTriangleEqual;",
	"&LeftUpDownVector;",
	"&LeftUpTeeVector;",
	"&LeftUpVector;",
	"&LeftUpVectorBar;",
	"&LeftVector;",
	"&LeftVectorBar;",
	"&Leftarrow;",
	"&Leftrightarrow;",
	"&LessEqualGreater;",
	"&LessFullEqual;",
	"&LessGreater;",
	"&LessLess;",
	"&LessSlantEqual;",
	"&LessTilde;",
	"&Lfr;",
	"&Ll;",
	"&Lleftarrow;",
	"&Lmidot;",
	"&LongLeftArrow;",
	"&LongLeftRightArrow;",
	"&LongRightArrow;",
	"&Longleftarrow;",
	"&Longleftrightarrow;",
	"&Longrightarrow;",
	"&Lopf;",
	"&LowerLeftArrow;",
	"&LowerRightArrow;",
	"&Lscr;",
	"&Lsh;",
	"&Lstrok;",
	"&Lt;",
	"&Map;",
	"&Mcy;",
	"&MediumSpace;",
	"&Mellintrf;",
	"&Mfr;",
	"&MinusPlus;",
	"&Mopf;",
	"&Mscr;",
	"&Mu;",
	"&NJcy;",
	"&Nacute;",
	"&Ncaron;",
	"&Ncedil;",
	"&Ncy;",
	"&NegativeMediumSpace;",
	"&NegativeThickSpace;",
	"&NegativeThinSpace;",
	"&NegativeVeryThinSpace;",
	"&NestedGreaterGreater;",
	"&NestedLessLess;",
	"&NewLine;",
	"&Nfr;",
	"&NoBreak;",
	"&NonBreakingSpace;",
	"&Nopf;",
	"&Not;",
	"&NotCongruent;",
	"&NotCupCap;",
	"&NotDoubleVerticalBar;",
	"&NotElement;",
	"&NotEqual;",
	"&NotEqualTilde;",
	"&NotExists;",
	"&NotGreater;",
	"&NotGreaterEqual;",
	"&NotGreaterFullEqual;",
	"&NotGreaterGreater;",
	"&NotGreaterLess;",
	"&NotGreaterSlantEqual;",
	"&NotGreaterTilde;",
	"&NotHumpDownHump;",
	"&NotHumpEqual;",
	"&NotLeftTriangle;",
	"&NotLeftTriangleBar;",
	"&NotLeftTriangleEqual;",
	"&NotLess;",
	"&NotLessEqual;",
	"&NotLessGreater;",
	"&NotLessLess;",
	"&NotLessSlantEqual;",
	"&NotLessTilde;",
	"&NotNestedGreaterGreater;",
	"&NotNestedLessLess;",
	"&NotPrecedes;",
	"&NotPrecedesEqual;",
	"&NotPrecedesSlantEqual;",
	"&NotReverseElement;",
	"&NotRightTriangle;",
	"&NotRightTriangleBar;",
	"&NotRightTriangleEqual;",
	"&NotSquareSubset;",
	"&NotSquareSubsetEqual;",
	"&NotSquareSuperset;",
	"&NotSquareSupersetEqual;",
	"&NotSubset;",
	"&NotSubsetEqual;",
	"&NotSucceeds;",
	"&NotSucceedsEqual;",
	"&NotSucceedsSlantEqual;",
	"&NotSucceedsTilde;",
	"&NotSuperset;",
	"&NotSupersetEqual;",
	"&NotTilde;",
	"&NotTildeEqual;",
	"&NotTildeFullEqual;",
	"&NotTildeTilde;",
	"&NotVerticalBar;",
	"&Nscr;",
	"&Ntilde;",
	"&Nu;",
	"&OElig;",
	"&Oacute;",
	"&Ocirc;",
	"&Ocy;",
	"&Odblac;",
	"&Ofr;",
	"&Ograve;",
	"&Omacr;",
	"&Omega;",
	"&Omicron;",
	"&Oopf;",
	"&OpenCurlyDoubleQuote;",
	"&OpenCurlyQuote;",
	"&Or;",
	"&Oscr;",
	"&Oslash;",
	"&Otilde;",
	"&Otimes;",
	"&Ouml;",
	"&OverBar;",
	"&OverBrace;",
	"&OverBracket;",
	"&OverParenthesis;",
	"&PartialD;",
	"&Pcy;",
	"&Pfr;",
	"&Phi;",
	"&Pi;",
	"&PlusMinus;",
	"&Poincareplane;",
	"&Popf;",
	"&Pr;",
	"&Precedes;",
	"&PrecedesEqual;",
	"&PrecedesSlantEqual;",
	"&PrecedesTilde;",
	"&Prime;",
	"&Product;",
	"&Proportion;",
	"&Proportional;",
	"&Pscr;",
	"&Psi;",
	"&QUOT;",
	"&Qfr;",
	"&Qopf;",
	"&Qscr;",
	"&RBarr;",
	"&REG;",
	"&Racute;",
	"&Rang;",
	"&Rarr;",
	"&Rarrtl;",
	"&Rcaron;",
	"&Rcedil;",
	"&Rcy;",
	"&Re;",
	"&ReverseElement;",
	"&ReverseEquilibrium;",
	"&ReverseUpEquilibrium;",
	"&Rfr;",
	"&Rho;",
	"&RightAngleBracket;",
	"&RightArrow;",
	"&RightArrowBar;",
	"&RightArrowLeftArrow;",
	"&RightCeiling;",
	"&RightDoubleBracket;",
	"&RightDownTeeVector;",
	"&RightDownVector;",
	"&RightDownVectorBar;",
	"&RightFloor;",
	"&RightTee;",
	"&RightTeeArrow;",
	"&RightTeeVector;",
	"&RightTriangle;",
	"&RightTriangleBar;",
	"&RightTriangleEqual;",
	"&RightUpDownVector;",
	"&RightUpTeeVector;",
	"&RightUpVector;",
	"&RightUpVectorBar;",
	"&RightVector;",
	"&RightVectorBar;",
	"&Rightarrow;",
	"&Ropf;",
	"&RoundImplies;",
	"&Rrightarrow;",
	"&Rscr;",
	"&Rsh;",
	"&RuleDelayed;",
	"&SHCHcy;",
	"&SHcy;",
	"&SOFTcy;",
	"&Sacute;",
	"&Sc;",
	"&Scaron;",
	"&Scedil;",
	"&Scirc;",
	"&Scy;",
	"&Sfr;",
	"&ShortDownArrow;",
	"&ShortLeftArrow;",
	"&ShortRightArrow;",
	"&ShortUpArrow;",
	"&Sigma;",
	"&SmallCircle;",
	"&Sopf;",
	"&Sqrt;",
	"&Square;",
	"&SquareIntersection;",
	"&SquareSubset;",
	"&SquareSubsetEqual;",
	"&SquareSuperset;",
	"&SquareSupersetEqual;",
	"&SquareUnion;",
	"&Sscr;",
	"&Star;",
	"&Sub;",
	"&Subset;",
	"&SubsetEqual;",
	"&Succeeds;",
	"&SucceedsEqual;",
	"&SucceedsSlantEqual;",
	"&SucceedsTilde;",
	"&SuchThat;",
	"&Sum;",
	"&Sup;",
	"&Superset;",
	"&SupersetEqual;",
	"&Supset;",
	"&THORN;",
	"&TRADE;",
	"&TSHcy;",
	"&TScy;",
	"&Tab;",
	"&Tau;",
	"&Tcaron;",
	"&Tcedil;",
	"&Tcy;",
	"&Tfr;",
	"&Therefore;",
	"&Theta;",
	"&ThickSpace;",
	"&ThinSpace;",
	"&Tilde;",
	"&TildeEqual;",
	"&TildeFullEqual;",
	"&TildeTilde;",
	"&Topf;",
	"&TripleDot;",
	"&Tscr;",
	"&Tstrok;",
	"&Uacute;",
	"&Uarr;",
	"&Uarrocir;",
	"&Ubrcy;",
	"&Ubreve;",
	"&Ucirc;",
	"&Ucy;",
	"&Udblac;",
	"&Ufr;",
	"&Ugrave;",
	"&Umacr;",
	"&UnderBar;",
	"&UnderBrace;",
	"&UnderBracket;",
	"&UnderParenthesis;",
	"&Union;",
	"&UnionPlus;",
	"&Uogon;",
	"&Uopf;",
	"&UpArrow;",
	"&UpArrowBar;",
	"&UpArrowDownArrow;",
	"&UpDownArrow;",
	"&UpEquilibrium;",
	"&UpTee;",
	"&UpTeeArrow;",
	"&Uparrow;",
	"&Updownarrow;",
	"&UpperLeftArrow;",
	"&UpperRightArrow;",
	"&Upsi;",
	"&Upsilon;",
	"&Uring;",
	"&Uscr;",
	"&Utilde;",
	"&Uuml;",
	"&VDash;",
	"&Vbar;",
	"&Vcy;",
	"&Vdash;",
	"&Vdashl;",
	"&Vee;",
	"&Verbar;",
	"&Vert;",
	"&VerticalBar;",
	"&VerticalLine;",
	"&VerticalSeparator;",
	"&VerticalTilde;",
	"&VeryThinSpace;",
	"&Vfr;",
	"&Vopf;",
	"&Vscr;",
	"&Vvdash;",
	"&Wcirc;",
	"&Wedge;",
	"&Wfr;",
	"&Wopf;",
	"&Wscr;",
	"&Xfr;",
	"&Xi;",
	"&Xopf;",
	"&Xscr;",
	"&YAcy;",
	"&YIcy;",
	"&YUcy;",
	"&Yacute;",
	"&Ycirc;",
	"&Ycy;",
	"&Yfr;",
	"&Yopf;",
	"&Yscr;",
	"&Yuml;",
	"&ZHcy;",
	"&Zacute;",
	"&Zcaron;",
	"&Zcy;",
	"&Zdot;",
	"&ZeroWidthSpace;",
	"&Zeta;",
	"&Zfr;",
	"&Zopf;",
	"&Zscr;",
	"&aacute;",
	"&abreve;",
	"&ac;",
	"&acE;",
	"&acd;",
	"&acirc;",
	"&acute;",
	"&acy;",
	"&aelig;",
	"&af;",
	"&afr;",
	"&agrave;",
	"&alefsym;",
	"&aleph;",
	"&alpha;",
	"&amacr;",
	"&amalg;",
	"&amp;",
	"&and;",
	"&andand;",
	"&andd;",
	"&andslope;",
	"&andv;",
	"&ang;",
	"&ange;",
	"&angle;",
	"&angmsd;",
	"&angmsdaa;",
	"&angmsdab;",
	"&angmsdac;",
	"&angmsdad;",
	"&angmsdae;",
	"&angmsdaf;",
	"&angmsdag;",
	"&angmsdah;",
	"&angrt;",
	"&angrtvb;",
	"&angrtvbd;",
	"&angsph;",
	"&angst;",
	"&angzarr;",
	"&aogon;",
	"&aopf;",
	"&ap;",
	"&apE;",
	"&apacir;",
	"&ape;",
	"&apid;",
	"&apos;",
	"&approx;",
	"&approxeq;",
	"&aring;",
	"&ascr;",
	"&ast;",
	"&asymp;",
	"&asympeq;",
	"&atilde;",
	"&auml;",
	"&awconint;",
	"&awint;",
	"&bNot;",
	"&backcong;",
	"&backepsilon;",
	"&backprime;",
	"&backsim;",
	"&backsimeq;",
	"&barvee;",
	"&barwed;",
	"&barwedge;",
	"&bbrk;",
	"&bbrktbrk;",
	"&bcong;",
	"&bcy;",
	"&bdquo;",
	"&becaus;",
	"&because;",
	"&bemptyv;",
	"&bepsi;",
	"&bernou;",
	"&beta;",
	"&beth;",
	"&between;",
	"&bfr;",
	"&bigcap;",
	"&bigcirc;",
	"&bigcup;",
	"&bigodot;",
	"&bigoplus;",
	"&bigotimes;",
	"&bigsqcup;",
	"&bigstar;",
	"&bigtriangledown;",
	"&bigtriangleup;",
	"&biguplus;",
	"&bigvee;",
	"&bigwedge;",
	"&bkarow;",
	"&blacklozenge;",
	"&blacksquare;",
	"&blacktriangle;",
	"&blacktriangledown;",
	"&blacktriangleleft;",
	"&blacktriangleright;",
	"&blank;",
	"&blk12;",
	"&blk14;",
	"&blk34;",
	"&block;",
	"&bne;",
	"&bnequiv;",
	"&bnot;",
	"&bopf;",
	"&bot;",
	"&bottom;",
	"&bowtie;",
	"&boxDL;",
	"&boxDR;",
	"&boxDl;",
	"&boxDr;",
	"&boxH;",
	"&boxHD;",
	"&boxHU;",
	"&boxHd;",
	"&boxHu;",
	"&boxUL;",
	"&boxUR;",
	"&boxUl;",
	"&boxUr;",
	"&boxV;",
	"&boxVH;",
	"&boxVL;",
	"&boxVR;",
	"&boxVh;",
	"&boxVl;",
	"&boxVr;",
	"&boxbox;",
	"&boxdL;",
	"&boxdR;",
	"&boxdl;",
	"&boxdr;",
	"&boxh;",
	"&boxhD;",
	"&boxhU;",
	"&boxhd;",
	"&boxhu;",
	"&boxminus;",
	"&boxplus;",
	"&boxtimes;",
	"&boxuL;",
	"&boxuR;",
	"&boxul;",
	"&boxur;",
	"&boxv;",
	"&boxvH;",
	"&boxvL;",
	"&boxvR;",
	"&boxvh;",
	"&boxvl;",
	"&boxvr;",
	"&bprime;",
	"&breve;",
	"&brvbar;",
	"&bscr;",
	"&bsemi;",
	"&bsim;",
	"&bsime;",
	"&bsol;",
	"&bsolb;",
	"&bsolhsub;",
	"&bull;",
	"&bullet;",
	"&bump;",
	"&bumpE;",
	"&bumpe;",
	"&bumpeq;",
	"&cacute;",
	"&cap;",
	"&capand;",
	"&capbrcup;",
	"&capcap;",
	"&capcup;",
	"&capdot;",
	"&caps;",
	"&caret;",
	"&caron;",
	"&ccaps;",
	"&ccaron;",
	"&ccedil;",
	"&ccirc;",
	"&ccups;",
	"&ccupssm;",
	"&cdot;",
	"&cedil;",
	"&cemptyv;",
	"&cent;",
	"&centerdot;",
	"&cfr;",
	"&chcy;",
	"&check;",
	"&checkmark;",
	"&chi;",
	"&cir;",
	"&cirE;",
	"&circ;",
	"&circeq;",
	"&circlearrowleft;",
	"&circlearrowright;",
	"&circledR;",
	"&circledS;",
	"&circledast;",
	"&circledcirc;",
	"&circleddash;",
	"&cire;",
	"&cirfnint;",
	"&cirmid;",
	"&cirscir;",
	"&clubs;",
	"&clubsuit;",
	"&colon;",
	"&colone;",
	"&coloneq;",
	"&comma;",
	"&commat;",
	"&comp;",
	"&compfn;",
	"&complement;",
	"&complexes;",
	"&cong;",
	"&congdot;",
	"&conint;",
	"&copf;",
	"&coprod;",
	"&copy;",
	"&copysr;",
	"&crarr;",
	"&cross;",
	"&cscr;",
	"&csub;",
	"&csube;",
	"&csup;",
	"&csupe;",
	"&ctdot;",
	"&cudarrl;",
	"&cudarrr;",
	"&cuepr;",
	"&cuesc;",
	"&cularr;",
	"&cularrp;",
	"&cup;",
	"&cupbrcap;",
	"&cupcap;",
	"&cupcup;",
	"&cupdot;",
	"&cupor;",
	"&cups;",
	"&curarr;",
	"&curarrm;",
	"&curlyeqprec;",
	"&curlyeqsucc;",
	"&curlyvee;",
	"&curlywedge;",
	"&curren;",
	"&curvearrowleft;",
	"&curvearrowright;",
	"&cuvee;",
	"&cuwed;",
	"&cwconint;",
	"&cwint;",
	"&cylcty;",
	"&dArr;",
	"&dHar;",
	"&dagger;",
	"&daleth;",
	"&darr;",
	"&dash;",
	"&dashv;",
	"&dbkarow;",
	"&dblac;",
	"&dcaron;",
	"&dcy;",
	"&dd;",
	"&ddagger;",
	"&ddarr;",
	"&ddotseq;",
	"&deg;",
	"&delta;",
	"&demptyv;",
	"&dfisht;",
	"&dfr;",
	"&dharl;",
	"&dharr;",
	"&diam;",
	"&diamond;",
	"&diamondsuit;",
	"&diams;",
	"&die;",
	"&digamma;",
	"&disin;",
	"&div;",
	"&divide;",
	"&divideontimes;",
	"&divonx;",
	"&djcy;",
	"&dlcorn;",
	"&dlcrop;",
	"&dollar;",
	"&dopf;",
	"&dot;",
	"&doteq;",
	"&doteqdot;",
	"&dotminus;",
	"&dotplus;",
	"&dotsquare;",
	"&doublebarwedge;",
	"&downarrow;",
	"&downdownarrows;",
	"&downharpoonleft;",
	"&downharpoonright;",
	"&drbkarow;",
	"&drcorn;",
	"&drcrop;",
	"&dscr;",
	"&dscy;",
	"&dsol;",
	"&dstrok;",
	"&dtdot;",
	"&dtri;",
	"&dtrif;",
	"&duarr;",
	"&duhar;",
	"&dwangle;",
	"&dzcy;",
	"&dzigrarr;",
	"&eDDot;",
	"&eDot;",
	"&eacute;",
	"&easter;",
	"&ecaron;",
	"&ecir;",
	"&ecirc;",
	"&ecolon;",
	"&ecy;",
	"&edot;",
	"&ee;",
	"&efDot;",
	"&efr;",
	"&eg;",
	"&egrave;",
	"&egs;",
	"&egsdot;",
	"&el;",
	"&elinters;",
	"&ell;",
	"&els;",
	"&elsdot;",
	"&emacr;",
	"&empty;",
	"&emptyset;",
	"&emptyv;",
	"&emsp;",
	"&emsp13;",
	"&emsp14;",
	"&eng;",
	"&ensp;",
	"&eogon;",
	"&eopf;",
	"&epar;",
	"&eparsl;",
	"&eplus;",
	"&epsi;",
	"&epsilon;",
	"&epsiv;",
	"&eqcirc;",
	"&eqcolon;",
	"&eqsim;",
	"&eqslantgtr;",
	"&eqslantless;",
	"&equals;",
	"&equest;",
	"&equiv;",
	"&equivDD;",
	"&eqvparsl;",
	"&erDot;",
	"&erarr;",
	"&escr;",
	"&esdot;",
	"&esim;",
	"&eta;",
	"&eth;",
	"&euml;",
	"&euro;",
	"&excl;",
	"&exist;",
	"&expectation;",
	"&exponentiale;",
	"&fallingdotseq;",
	"&fcy;",
	"&female;",
	"&ffilig;",
	"&fflig;",
	"&ffllig;",
	"&ffr;",
	"&filig;",
	"&fjlig;",
	"&flat;",
	"&fllig;",
	"&fltns;",
	"&fnof;",
	"&fopf;",
	"&forall;",
	"&fork;",
	"&forkv;",
	"&fpartint;",
	"&frac12;",
	"&frac13;",
	"&frac14;",
	"&frac15;",
	"&frac16;",
	"&frac18;",
	"&frac23;",
	"&frac25;",
	"&frac34;",
	"&frac35;",
	"&frac38;",
	"&frac45;",
	"&frac56;",
	"&frac58;",
	"&frac78;",
	"&frasl;",
	"&frown;",
	"&fscr;",
	"&gE;",
	"&gEl;",
	"&gacute;",
	"&gamma;",
	"&gammad;",
	"&gap;",
	"&gbreve;",
	"&gcirc;",
	"&gcy;",
	"&gdot;",
	"&ge;",
	"&gel;",
	"&geq;",
	"&geqq;",
	"&geqslant;",
	"&ges;",
	"&gescc;",
	"&gesdot;",
	"&gesdoto;",
	"&gesdotol;",
	"&gesl;",
	"&gesles;",
	"&gfr;",
	"&gg;",
	"&ggg;",
	"&gimel;",
	"&gjcy;",
	"&gl;",
	"&glE;",
	"&gla;",
	"&glj;",
	"&gnE;",
	"&gnap;",
	"&gnapprox;",
	"&gne;",
	"&gneq;",
	"&gneqq;",
	"&gnsim;",
	"&gopf;",
	"&grave;",
	"&gscr;",
	"&gsim;",
	"&gsime;",
	"&gsiml;",
	"&gt;",
	"&gtcc;",
	"&gtcir;",
	"&gtdot;",
	"&gtlPar;",
	"&gtquest;",
	"&gtrapprox;",
	"&gtrarr;",
	"&gtrdot;",
	"&gtreqless;",
	"&gtreqqless;",
	"&gtrless;",
	"&gtrsim;",
	"&gvertneqq;",
	"&gvnE;",
	"&hArr;",
	"&hairsp;",
	"&half;",
	"&hamilt;",
	"&hardcy;",
	"&harr;",
	"&harrcir;",
	"&harrw;",
	"&hbar;",
	"&hcirc;",
	"&hearts;",
	"&heartsuit;",
	"&hellip;",
	"&hercon;",
	"&hfr;",
	"&hksearow;",
	"&hkswarow;",
	"&hoarr;",
	"&homtht;",
	"&hookleftarrow;",
	"&hookrightarrow;",
	"&hopf;",
	"&horbar;",
	"&hscr;",
	"&hslash;",
	"&hstrok;",
	"&hybull;",
	"&hyphen;",
	"&iacute;",
	"&ic;",
	"&icirc;",
	"&icy;",
	"&iecy;",
	"&iexcl;",
	"&iff;",
	"&ifr;",
	"&igrave;",
	"&ii;",
	"&iiiint;",
	"&iiint;",
	"&iinfin;",
	"&iiota;",
	"&ijlig;",
	"&imacr;",
	"&image;",
	"&imagline;",
	"&imagpart;",
	"&imath;",
	"&imof;",
	"&imped;",
	"&in;",
	"&incare;",
	"&infin;",
	"&infintie;",
	"&inodot;",
	"&int;",
	"&intcal;",
	"&integers;",
	"&intercal;",
	"&intlarhk;",
	"&intprod;",
	"&iocy;",
	"&iogon;",
	"&iopf;",
	"&iota;",
	"&iprod;",
	"&iquest;",
	"&iscr;",
	"&isin;",
	"&isinE;",
	"&isindot;",
	"&isins;",
	"&isinsv;",
	"&isinv;",
	"&it;",
	"&itilde;",
	"&iukcy;",
	"&iuml;",
	"&jcirc;",
	"&jcy;",
	"&jfr;",
	"&jmath;",
	"&jopf;",
	"&jscr;",
	"&jsercy;",
	"&jukcy;",
	"&kappa;",
	"&kappav;",
	"&kcedil;",
	"&kcy;",
	"&kfr;",
	"&kgreen;",
	"&khcy;",
	"&kjcy;",
	"&kopf;",
	"&kscr;",
	"&lAarr;",
	"&lArr;",
	"&lAtail;",
	"&lBarr;",
	"&lE;",
	"&lEg;",
	"&lHar;",
	"&lacute;",
	"&laemptyv;",
	"&lagran;",
	"&lambda;",
	"&lang;",
	"&langd;",
	"&langle;",
	"&lap;",
	"&laquo;",
	"&larr;",
	"&larrb;",
	"&larrbfs;",
	"&larrfs;",
	"&larrhk;",
	"&larrlp;",
	"&larrpl;",
	"&larrsim;",
	"&larrtl;",
	"&lat;",
	"&latail;",
	"&late;",
	"&lates;",
	"&lbarr;",
	"&lbbrk;",
	"&lbrace;",
	"&lbrack;",
	"&lbrke;",
	"&lbrksld;",
	"&lbrkslu;",
	"&lcaron;",
	"&lcedil;",
	"&lceil;",
	"&lcub;",
	"&lcy;",
	"&ldca;",
	"&ldquo;",
	"&ldquor;",
	"&ldrdhar;",
	"&ldrushar;",
	"&ldsh;",
	"&le;",
	"&leftarrow;",
	"&leftarrowtail;",
	"&leftharpoondown;",
	"&leftharpoonup;",
	"&leftleftarrows;",
	"&leftrightarrow;",
	"&leftrightarrows;",
	"&leftrightharpoons;",
	"&leftrightsquigarrow;",
	"&leftthreetimes;",
	"&leg;",
	"&leq;",
	"&leqq;",
	"&leqslant;",
	"&les;",
	"&lescc;",
	"&lesdot;",
	"&lesdoto;",
	"&lesdotor;",
	"&lesg;",
	"&lesges;",
	"&lessapprox;",
	"&lessdot;",
	"&lesseqgtr;",
	"&lesseqqgtr;",
	"&lessgtr;",
	"&lesssim;",
	"&lfisht;",
	"&lfloor;",
	"&lfr;",
	"&lg;",
	"&lgE;",
	"&lhard;",
	"&lharu;",
	"&lharul;",
	"&lhblk;",
	"&ljcy;",
	"&ll;",
	"&llarr;",
	"&llcorner;",
	"&llhard;",
	"&lltri;",
	"&lmidot;",
	"&lmoust;",
	"&lmoustache;",
	"&lnE;",
	"&lnap;",
	"&lnapprox;",
	"&lne;",
	"&lneq;",
	"&lneqq;",
	"&lnsim;",
	"&loang;",
	"&loarr;",
	"&lobrk;",
	"&longleftarrow;",
	"&longleftrightarrow;",
	"&longmapsto;",
	"&longrightarrow;",
	"&looparrowleft;",
	"&looparrowright;",
	"&lopar;",
	"&lopf;",
	"&loplus;",
	"&lotimes;",
	"&lowast;",
	"&lowbar;",
	"&loz;",
	"&lozenge;",
	"&lozf;",
	"&lpar;",
	"&lparlt;",
	"&lrarr;",
	"&lrcorner;",
	"&lrhar;",
	"&lrhard;",
	"&lrm;",
	"&lrtri;",
	"&lsaquo;",
	"&lscr;",
	"&lsh;",
	"&lsim;",
	"&lsime;",
	"&lsimg;",
	"&lsqb;",
	"&lsquo;",
	"&lsquor;",
	"&lstrok;",
	"&lt;",
	"&ltcc;",
	"&ltcir;",
	"&ltdot;",
	"&lthree;",
	"&ltimes;",
	"&ltlarr;",
	"&ltquest;",
	"&ltrPar;",
	"&ltri;",
	"&ltrie;",
	"&ltrif;",
	"&lurdshar;",
	"&luruhar;",
	"&lvertneqq;",
	"&lvnE;",
	"&mDDot;",
	"&macr;",
	"&male;",
	"&malt;",
	"&maltese;",
	"&map;",
	"&mapsto;",
	"&mapstodown;",
	"&mapstoleft;",
	"&mapstoup;",
	"&marker;",
	"&mcomma;",
	"&mcy;",
	"&mdash;",
	"&measuredangle;",
	"&mfr;",
	"&mho;",
	"&micro;",
	"&mid;",
	"&midast;",
	"&midcir;",
	"&middot;",
	"&minus;",
	"&minusb;",
	"&minusd;",
	"&minusdu;",
	"&mlcp;",
	"&mldr;",
	"&mnplus;",
	"&models;",
	"&mopf;",
	"&mp;",
	"&mscr;",
	"&mstpos;",
	"&mu;",
	"&multimap;",
	"&mumap;",
	"&nGg;",
	"&nGt;",
	"&nGtv;",
	"&nLeftarrow;",
	"&nLeftrightarrow;",
	"&nLl;",
	"&nLt;",
	"&nLtv;",
	"&nRightarrow;",
	"&nVDash;",
	"&nVdash;",
	"&nabla;",
	"&nacute;",
	"&nang;",
	"&nap;",
	"&napE;",
	"&napid;",
	"&napos;",
	"&napprox;",
	"&natur;",
	"&natural;",
	"&naturals;",
	"&nbsp;",
	"&nbump;",
	"&nbumpe;",
	"&ncap;",
	"&ncaron;",
	"&ncedil;",
	"&ncong;",
	"&ncongdot;",
	"&ncup;",
	"&ncy;",
	"&ndash;",
	"&ne;",
	"&neArr;",
	"&nearhk;",
	"&nearr;",
	"&nearrow;",
	"&nedot;",
	"&nequiv;",
	"&nesear;",
	"&nesim;",
	"&nexist;",
	"&nexists;",
	"&nfr;",
	"&ngE;",
	"&nge;",
	"&ngeq;",
	"&ngeqq;",
	"&ngeqslant;",
	"&nges;",
	"&ngsim;",
	"&ngt;",
	"&ngtr;",
	"&nhArr;",
	"&nharr;",
	"&nhpar;",
	"&ni;",
	"&nis;",
	"&nisd;",
	"&niv;",
	"&njcy;",
	"&nlArr;",
	"&nlE;",
	"&nlarr;",
	"&nldr;",
	"&nle;",
	"&nleftarrow;",
	"&nleftrightarrow;",
	"&nleq;",
	"&nleqq;",
	"&nleqslant;",
	"&nles;",
	"&nless;",
	"&nlsim;",
	"&nlt;",
	"&nltri;",
	"&nltrie;",
	"&nmid;",
	"&nopf;",
	"&not;",
	"&notin;",
	"&notinE;",
	"&notindot;",
	"&notinva;",
	"&notinvb;",
	"&notinvc;",
	"&notni;",
	"&notniva;",
	"&notnivb;",
	"&notnivc;",
	"&npar;",
	"&nparallel;",
	"&nparsl;",
	"&npart;",
	"&npolint;",
	"&npr;",
	"&nprcue;",
	"&npre;",
	"&nprec;",
	"&npreceq;",
	"&nrArr;",
	"&nrarr;",
	"&nrarrc;",
	"&nrarrw;",
	"&nrightarrow;",
	"&nrtri;",
	"&nrtrie;",
	"&nsc;",
	"&nsccue;",
	"&nsce;",
	"&nscr;",
	"&nshortmid;",
	"&nshortparallel;",
	"&nsim;",
	"&nsime;",
	"&nsimeq;",
	"&nsmid;",
	"&nspar;",
	"&nsqsube;",
	"&nsqsupe;",
	"&nsub;",
	"&nsubE;",
	"&nsube;",
	"&nsubset;",
	"&nsubseteq;",
	"&nsubseteqq;",
	"&nsucc;",
	"&nsucceq;",
	"&nsup;",
	"&nsupE;",
	"&nsupe;",
	"&nsupset;",
	"&nsupseteq;",
	"&nsupseteqq;",
	"&ntgl;",
	"&ntilde;",
	"&ntlg;",
	"&ntriangleleft;",
	"&ntrianglelefteq;",
	"&ntriangleright;",
	"&ntrianglerighteq;",
	"&nu;",
	"&num;",
	"&numero;",
	"&numsp;",
	"&nvDash;",
	"&nvHarr;",
	"&nvap;",
	"&nvdash;",
	"&nvge;",
	"&nvgt;",
	"&nvinfin;",
	"&nvlArr;",
	"&nvle;",
	"&nvlt;",
	"&nvltrie;",
	"&nvrArr;",
	"&nvrtrie;",
	"&nvsim;",
	"&nwArr;",
	"&nwarhk;",
	"&nwarr;",
	"&nwarrow;",
	"&nwnear;",
	"&oS;",
	"&oacute;",
	"&oast;",
	"&ocir;",
	"&ocirc;",
	"&ocy;",
	"&odash;",
	"&odblac;",
	"&odiv;",
	"&odot;",
	"&odsold;",
	"&oelig;",
	"&ofcir;",
	"&ofr;",
	"&ogon;",
	"&ograve;",
	"&ogt;",
	"&ohbar;",
	"&ohm;",
	"&oint;",
	"&olarr;",
	"&olcir;",
	"&olcross;",
	"&oline;",
	"&olt;",
	"&omacr;",
	"&omega;",
	"&omicron;",
	"&omid;",
	"&ominus;",
	"&oopf;",
	"&opar;",
	"&operp;",
	"&oplus;",
	"&or;",
	"&orarr;",
	"&ord;",
	"&order;",
	"&orderof;",
	"&ordf;",
	"&ordm;",
	"&origof;",
	"&oror;",
	"&orslope;",
	"&orv;",
	"&oscr;",
	"&oslash;",
	"&osol;",
	"&otilde;",
	"&otimes;",
	"&otimesas;",
	"&ouml;",
	"&ovbar;",
	"&par;",
	"&para;",
	"&parallel;",
	"&parsim;",
	"&parsl;",
	"&part;",
	"&pcy;",
	"&percnt;",
	"&period;",
	"&permil;",
	"&perp;",
	"&pertenk;",
	"&pfr;",
	"&phi;",
	"&phiv;",
	"&phmmat;",
	"&phone;",
	"&pi;",
	"&pitchfork;",
	"&piv;",
	"&planck;",
	"&planckh;",
	"&plankv;",
	"&plus;",
	"&plusacir;",
	"&plusb;",
	"&pluscir;",
	"&plusdo;",
	"&plusdu;",
	"&pluse;",
	"&plusmn;",
	"&plussim;",
	"&plustwo;",
	"&pm;",
	"&pointint;",
	"&popf;",
	"&pound;",
	"&pr;",
	"&prE;",
	"&prap;",
	"&prcue;",
	"&pre;",
	"&prec;",
	"&precapprox;",
	"&preccurlyeq;",
	"&preceq;",
	"&precnapprox;",
	"&precneqq;",
	"&precnsim;",
	"&precsim;",
	"&prime;",
	"&primes;",
	"&prnE;",
	"&prnap;",
	"&prnsim;",
	"&prod;",
	"&profalar;",
	"&profline;",
	"&profsurf;",
	"&prop;",
	"&propto;",
	"&prsim;",
	"&prurel;",
	"&pscr;",
	"&psi;",
	"&puncsp;",
	"&qfr;",
	"&qint;",
	"&qopf;",
	"&qprime;",
	"&qscr;",
	"&quaternions;",
	"&quatint;",
	"&quest;",
	"&questeq;",
	"&quot;",
	"&rAarr;",
	"&rArr;",
	"&rAtail;",
	"&rBarr;",
	"&rHar;",
	"&race;",
	"&racute;",
	"&radic;",
	"&raemptyv;",
	"&rang;",
	"&rangd;",
	"&range;",
	"&rangle;",
	"&raquo;",
	"&rarr;",
	"&rarrap;",
	"&rarrb;",
	"&rarrbfs;",
	"&rarrc;",
	"&rarrfs;",
	"&rarrhk;",
	"&rarrlp;",
	"&rarrpl;",
	"&rarrsim;",
	"&rarrtl;",
	"&rarrw;",
	"&ratail;",
	"&ratio;",
	"&rationals;",
	"&rbarr;",
	"&rbbrk;",
	"&rbrace;",
	"&rbrack;",
	"&rbrke;",
	"&rbrksld;",
	"&rbrkslu;",
	"&rcaron;",
	"&rcedil;",
	"&rceil;",
	"&rcub;",
	"&rcy;",
	"&rdca;",
	"&rdldhar;",
	"&rdquo;",
	"&rdquor;",
	"&rdsh;",
	"&real;",
	"&realine;",
	"&realpart;",
	"&reals;",
	"&rect;",
	"&reg;",
	"&rfisht;",
	"&rfloor;",
	"&rfr;",
	"&rhard;",
	"&rharu;",
	"&rharul;",
	"&rho;",
	"&rhov;",
	"&rightarrow;",
	"&rightarrowtail;",
	"&rightharpoondown;",
	"&rightharpoonup;",
	"&rightleftarrows;",
	"&rightleftharpoons;",
	"&rightrightarrows;",
	"&rightsquigarrow;",
	"&rightthreetimes;",
	"&ring;",
	"&risingdotseq;",
	"&rlarr;",
	"&rlhar;",
	"&rlm;",
	"&rmoust;",
	"&rmoustache;",
	"&rnmid;",
	"&roang;",
	"&roarr;",
	"&robrk;",
	"&ropar;",
	"&ropf;",
	"&roplus;",
	"&rotimes;",
	"&rpar;",
	"&rpargt;",
	"&rppolint;",
	"&rrarr;",
	"&rsaquo;",
	"&rscr;",
	"&rsh;",
	"&rsqb;",
	"&rsquo;",
	"&rsquor;",
	"&rthree;",
	"&rtimes;",
	"&rtri;",
	"&rtrie;",
	"&rtrif;",
	"&rtriltri;",
	"&ruluhar;",
	"&rx;",
	"&sacute;",
	"&sbquo;",
	"&sc;",
	"&scE;",
	"&scap;",
	"&scaron;",
	"&sccue;",
	"&sce;",
	"&scedil;",
	"&scirc;",
	"&scnE;",
	"&scnap;",
	"&scnsim;",
	"&scpolint;",
	"&scsim;",
	"&scy;",
	"&sdot;",
	"&sdotb;",
	"&sdote;",
	"&seArr;",
	"&searhk;",
	"&searr;",
	"&searrow;",
	"&sect;",
	"&semi;",
	"&seswar;",
	"&setminus;",
	"&setmn;",
	"&sext;",
	"&sfr;",
	"&sfrown;",
	"&sharp;",
	"&shchcy;",
	"&shcy;",
	"&shortmid;",
	"&shortparallel;",
	"&shy;",
	"&sigma;",
	"&sigmaf;",
	"&sigmav;",
	"&sim;",
	"&simdot;",
	"&sime;",
	"&simeq;",
	"&simg;",
	"&simgE;",
	"&siml;",
	"&simlE;",
	"&simne;",
	"&simplus;",
	"&simrarr;",
	"&slarr;",
	"&smallsetminus;",
	"&smashp;",
	"&smeparsl;",
	"&smid;",
	"&smile;",
	"&smt;",
	"&smte;",
	"&smtes;",
	"&softcy;",
	"&sol;",
	"&solb;",
	"&solbar;",
	"&sopf;",
	"&spades;",
	"&spadesuit;",
	"&spar;",
	"&sqcap;",
	"&sqcaps;",
	"&sqcup;",
	"&sqcups;",
	"&sqsub;",
	"&sqsube;",
	"&sqsubset;",
	"&sqsubseteq;",
	"&sqsup;",
	"&sqsupe;",
	"&sqsupset;",
	"&sqsupseteq;",
	"&squ;",
	"&square;",
	"&squarf;",
	"&squf;",
	"&srarr;",
	"&sscr;",
	"&ssetmn;",
	"&ssmile;",
	"&sstarf;",
	"&star;",
	"&starf;",
	"&straightepsilon;",
	"&straightphi;",
	"&strns;",
	"&sub;",
	"&subE;",
	"&subdot;",
	"&sube;",
	"&subedot;",
	"&submult;",
	"&subnE;",
	"&subne;",
	"&subplus;",
	"&subrarr;",
	"&subset;",
	"&subseteq;",
	"&subseteqq;",
	"&subsetneq;",
	"&subsetneqq;",
	"&subsim;",
	"&subsub;",
	"&subsup;",
	"&succ;",
	"&succapprox;",
	"&succcurlyeq;",
	"&succeq;",
	"&succnapprox;",
	"&succneqq;",
	"&succnsim;",
	"&succsim;",
	"&sum;",
	"&sung;",
	"&sup;",
	"&sup1;",
	"&sup2;",
	"&sup3;",
	"&supE;",
	"&supdot;",
	"&supdsub;",
	"&supe;",
	"&supedot;",
	"&suphsol;",
	"&suphsub;",
	"&suplarr;",
	"&supmult;",
	"&supnE;",
	"&supne;",
	"&supplus;",
	"&supset;",
	"&supseteq;",
	"&supseteqq;",
	"&supsetneq;",
	"&supsetneqq;",
	"&supsim;",
	"&supsub;",
	"&supsup;",
	"&swArr;",
	"&swarhk;",
	"&swarr;",
	"&swarrow;",
	"&swnwar;",
	"&szlig;",
	"&target;",
	"&tau;",
	"&tbrk;",
	"&tcaron;",
	"&tcedil;",
	"&tcy;",
	"&tdot;",
	"&telrec;",
	"&tfr;",
	"&there4;",
	"&therefore;",
	"&theta;",
	"&thetasym;",
	"&thetav;",
	"&thickapprox;",
	"&thicksim;",
	"&thinsp;",
	"&thkap;",
	"&thksim;",
	"&thorn;",
	"&tilde;",
	"&times;",
	"&timesb;",
	"&timesbar;",
	"&timesd;",
	"&tint;",
	"&toea;",
	"&top;",
	"&topbot;",
	"&topcir;",
	"&topf;",
	"&topfork;",
	"&tosa;",
	"&tprime;",
	"&trade;",
	"&triangle;",
	"&triangledown;",
	"&triangleleft;",
	"&trianglelefteq;",
	"&triangleq;",
	"&triangleright;",
	"&trianglerighteq;",
	"&tridot;",
	"&trie;",
	"&triminus;",
	"&triplus;",
	"&trisb;",
	"&tritime;",
	"&trpezium;",
	"&tscr;",
	"&tscy;",
	"&tshcy;",
	"&tstrok;",
	"&twixt;",
	"&twoheadleftarrow;",
	"&twoheadrightarrow;",
	"&uArr;",
	"&uHar;",
	"&uacute;",
	"&uarr;",
	"&ubrcy;",
	"&ubreve;",
	"&ucirc;",
	"&ucy;",
	"&udarr;",
	"&udblac;",
	"&udhar;",
	"&ufisht;",
	"&ufr;",
	"&ugrave;",
	"&uharl;",
	"&uharr;",
	"&uhblk;",
	"&ulcorn;",
	"&ulcorner;",
	"&ulcrop;",
	"&ultri;",
	"&umacr;",
	"&uml;",
	"&uogon;",
	"&uopf;",
	"&uparrow;",
	"&updownarrow;",
	"&upharpoonleft;",
	"&upharpoonright;",
	"&uplus;",
	"&upsi;",
	"&upsih;",
	"&upsilon;",
	"&upuparrows;",
	"&urcorn;",
	"&urcorner;",
	"&urcrop;",
	"&uring;",
	"&urtri;",
	"&uscr;",
	"&utdot;",
	"&utilde;",
	"&utri;",
	"&utrif;",
	"&uuarr;",
	"&uuml;",
	"&uwangle;",
	"&vArr;",
	"&vBar;",
	"&vBarv;",
	"&vDash;",
	"&vangrt;",
	"&varepsilon;",
	"&varkappa;",
	"&varnothing;",
	"&varphi;",
	"&varpi;",
	"&varpropto;",
	"&varr;",
	"&varrho;",
	"&varsigma;",
	"&varsubsetneq;",
	"&varsubsetneqq;",
	"&varsupsetneq;",
	"&varsupsetneqq;",
	"&vartheta;",
	"&vartriangleleft;",
	"&vartriangleright;",
	"&vcy;",
	"&vdash;",
	"&vee;",
	"&veebar;",
	"&veeeq;",
	"&vellip;",
	"&verbar;",
	"&vert;",
	"&vfr;",
	"&vltri;",
	"&vnsub;",
	"&vnsup;",
	"&vopf;",
	"&vprop;",
	"&vrtri;",
	"&vscr;",
	"&vsubnE;",
	"&vsubne;",
	"&vsupnE;",
	"&vsupne;",
	"&vzigzag;",
	"&wcirc;",
	"&wedbar;",
	"&wedge;",
	"&wedgeq;",
	"&weierp;",
	"&wfr;",
	"&wopf;",
	"&wp;",
	"&wr;",
	"&wreath;",
	"&wscr;",
	"&xcap;",
	"&xcirc;",
	"&xcup;",
	"&xdtri;",
	"&xfr;",
	"&xhArr;",
	"&xharr;",
	"&xi;",
	"&xlArr;",
	"&xlarr;",
	"&xmap;",
	"&xnis;",
	"&xodot;",
	"&xopf;",
	"&xoplus;",
	"&xotime;",
	"&xrArr;",
	"&xrarr;",
	"&xscr;",
	"&xsqcup;",
	"&xuplus;",
	"&xutri;",
	"&xvee;",
	"&xwedge;",
	"&yacute;",
	"&yacy;",
	"&ycirc;",
	"&ycy;",
	"&yen;",
	"&yfr;",
	"&yicy;",
	"&yopf;",
	"&yscr;",
	"&yucy;",
	"&yuml;",
	"&zacute;",
	"&zcaron;",
	"&zcy;",
	"&zdot;",
	"&zeetrf;",
	"&zeta;",
	"&zfr;",
	"&zhcy;",
	"&zigrarr;",
	"&zopf;",
	"&zscr;",
	"&zwj;",
	"&zwnj;",
);

foreach ($map as $str) {
    $de = html_entity_decode($str, ENT_QUOTES | ENT_HTML5, "UTF-8");
    echo "$str => $de ", bin2hex($de), "\n";
}
?>
--EXPECT--
&AElig; => Æ c386
&AMP; => & 26
&Aacute; => Á c381
&Abreve; => Ă c482
&Acirc; => Â c382
&Acy; => А d090
&Afr; => 𝔄 f09d9484
&Agrave; => À c380
&Alpha; => Α ce91
&Amacr; => Ā c480
&And; => ⩓ e2a993
&Aogon; => Ą c484
&Aopf; => 𝔸 f09d94b8
&ApplyFunction; => ⁡ e281a1
&Aring; => Å c385
&Ascr; => 𝒜 f09d929c
&Assign; => ≔ e28994
&Atilde; => Ã c383
&Auml; => Ä c384
&Backslash; => ∖ e28896
&Barv; => ⫧ e2aba7
&Barwed; => ⌆ e28c86
&Bcy; => Б d091
&Because; => ∵ e288b5
&Bernoullis; => ℬ e284ac
&Beta; => Β ce92
&Bfr; => 𝔅 f09d9485
&Bopf; => 𝔹 f09d94b9
&Breve; => ˘ cb98
&Bscr; => ℬ e284ac
&Bumpeq; => ≎ e2898e
&CHcy; => Ч d0a7
&COPY; => © c2a9
&Cacute; => Ć c486
&Cap; => ⋒ e28b92
&CapitalDifferentialD; => ⅅ e28585
&Cayleys; => ℭ e284ad
&Ccaron; => Č c48c
&Ccedil; => Ç c387
&Ccirc; => Ĉ c488
&Cconint; => ∰ e288b0
&Cdot; => Ċ c48a
&Cedilla; => ¸ c2b8
&CenterDot; => · c2b7
&Cfr; => ℭ e284ad
&Chi; => Χ cea7
&CircleDot; => ⊙ e28a99
&CircleMinus; => ⊖ e28a96
&CirclePlus; => ⊕ e28a95
&CircleTimes; => ⊗ e28a97
&ClockwiseContourIntegral; => ∲ e288b2
&CloseCurlyDoubleQuote; => ” e2809d
&CloseCurlyQuote; => ’ e28099
&Colon; => ∷ e288b7
&Colone; => ⩴ e2a9b4
&Congruent; => ≡ e289a1
&Conint; => ∯ e288af
&ContourIntegral; => ∮ e288ae
&Copf; => ℂ e28482
&Coproduct; => ∐ e28890
&CounterClockwiseContourIntegral; => ∳ e288b3
&Cross; => ⨯ e2a8af
&Cscr; => 𝒞 f09d929e
&Cup; => ⋓ e28b93
&CupCap; => ≍ e2898d
&DD; => ⅅ e28585
&DDotrahd; => ⤑ e2a491
&DJcy; => Ђ d082
&DScy; => Ѕ d085
&DZcy; => Џ d08f
&Dagger; => ‡ e280a1
&Darr; => ↡ e286a1
&Dashv; => ⫤ e2aba4
&Dcaron; => Ď c48e
&Dcy; => Д d094
&Del; => ∇ e28887
&Delta; => Δ ce94
&Dfr; => 𝔇 f09d9487
&DiacriticalAcute; => ´ c2b4
&DiacriticalDot; => ˙ cb99
&DiacriticalDoubleAcute; => ˝ cb9d
&DiacriticalGrave; => ` 60
&DiacriticalTilde; => ˜ cb9c
&Diamond; => ⋄ e28b84
&DifferentialD; => ⅆ e28586
&Dopf; => 𝔻 f09d94bb
&Dot; => ¨ c2a8
&DotDot; => ⃜ e2839c
&DotEqual; => ≐ e28990
&DoubleContourIntegral; => ∯ e288af
&DoubleDot; => ¨ c2a8
&DoubleDownArrow; => ⇓ e28793
&DoubleLeftArrow; => ⇐ e28790
&DoubleLeftRightArrow; => ⇔ e28794
&DoubleLeftTee; => ⫤ e2aba4
&DoubleLongLeftArrow; => ⟸ e29fb8
&DoubleLongLeftRightArrow; => ⟺ e29fba
&DoubleLongRightArrow; => ⟹ e29fb9
&DoubleRightArrow; => ⇒ e28792
&DoubleRightTee; => ⊨ e28aa8
&DoubleUpArrow; => ⇑ e28791
&DoubleUpDownArrow; => ⇕ e28795
&DoubleVerticalBar; => ∥ e288a5
&DownArrow; => ↓ e28693
&DownArrowBar; => ⤓ e2a493
&DownArrowUpArrow; => ⇵ e287b5
&DownBreve; => ̑ cc91
&DownLeftRightVector; => ⥐ e2a590
&DownLeftTeeVector; => ⥞ e2a59e
&DownLeftVector; => ↽ e286bd
&DownLeftVectorBar; => ⥖ e2a596
&DownRightTeeVector; => ⥟ e2a59f
&DownRightVector; => ⇁ e28781
&DownRightVectorBar; => ⥗ e2a597
&DownTee; => ⊤ e28aa4
&DownTeeArrow; => ↧ e286a7
&Downarrow; => ⇓ e28793
&Dscr; => 𝒟 f09d929f
&Dstrok; => Đ c490
&ENG; => Ŋ c58a
&ETH; => Ð c390
&Eacute; => É c389
&Ecaron; => Ě c49a
&Ecirc; => Ê c38a
&Ecy; => Э d0ad
&Edot; => Ė c496
&Efr; => 𝔈 f09d9488
&Egrave; => È c388
&Element; => ∈ e28888
&Emacr; => Ē c492
&EmptySmallSquare; => ◻ e297bb
&EmptyVerySmallSquare; => ▫ e296ab
&Eogon; => Ę c498
&Eopf; => 𝔼 f09d94bc
&Epsilon; => Ε ce95
&Equal; => ⩵ e2a9b5
&EqualTilde; => ≂ e28982
&Equilibrium; => ⇌ e2878c
&Escr; => ℰ e284b0
&Esim; => ⩳ e2a9b3
&Eta; => Η ce97
&Euml; => Ë c38b
&Exists; => ∃ e28883
&ExponentialE; => ⅇ e28587
&Fcy; => Ф d0a4
&Ffr; => 𝔉 f09d9489
&FilledSmallSquare; => ◼ e297bc
&FilledVerySmallSquare; => ▪ e296aa
&Fopf; => 𝔽 f09d94bd
&ForAll; => ∀ e28880
&Fouriertrf; => ℱ e284b1
&Fscr; => ℱ e284b1
&GJcy; => Ѓ d083
&GT; => > 3e
&Gamma; => Γ ce93
&Gammad; => Ϝ cf9c
&Gbreve; => Ğ c49e
&Gcedil; => Ģ c4a2
&Gcirc; => Ĝ c49c
&Gcy; => Г d093
&Gdot; => Ġ c4a0
&Gfr; => 𝔊 f09d948a
&Gg; => ⋙ e28b99
&Gopf; => 𝔾 f09d94be
&GreaterEqual; => ≥ e289a5
&GreaterEqualLess; => ⋛ e28b9b
&GreaterFullEqual; => ≧ e289a7
&GreaterGreater; => ⪢ e2aaa2
&GreaterLess; => ≷ e289b7
&GreaterSlantEqual; => ⩾ e2a9be
&GreaterTilde; => ≳ e289b3
&Gscr; => 𝒢 f09d92a2
&Gt; => ≫ e289ab
&HARDcy; => Ъ d0aa
&Hacek; => ˇ cb87
&Hat; => ^ 5e
&Hcirc; => Ĥ c4a4
&Hfr; => ℌ e2848c
&HilbertSpace; => ℋ e2848b
&Hopf; => ℍ e2848d
&HorizontalLine; => ─ e29480
&Hscr; => ℋ e2848b
&Hstrok; => Ħ c4a6
&HumpDownHump; => ≎ e2898e
&HumpEqual; => ≏ e2898f
&IEcy; => Е d095
&IJlig; => Ĳ c4b2
&IOcy; => Ё d081
&Iacute; => Í c38d
&Icirc; => Î c38e
&Icy; => И d098
&Idot; => İ c4b0
&Ifr; => ℑ e28491
&Igrave; => Ì c38c
&Im; => ℑ e28491
&Imacr; => Ī c4aa
&ImaginaryI; => ⅈ e28588
&Implies; => ⇒ e28792
&Int; => ∬ e288ac
&Integral; => ∫ e288ab
&Intersection; => ⋂ e28b82
&InvisibleComma; => ⁣ e281a3
&InvisibleTimes; => ⁢ e281a2
&Iogon; => Į c4ae
&Iopf; => 𝕀 f09d9580
&Iota; => Ι ce99
&Iscr; => ℐ e28490
&Itilde; => Ĩ c4a8
&Iukcy; => І d086
&Iuml; => Ï c38f
&Jcirc; => Ĵ c4b4
&Jcy; => Й d099
&Jfr; => 𝔍 f09d948d
&Jopf; => 𝕁 f09d9581
&Jscr; => 𝒥 f09d92a5
&Jsercy; => Ј d088
&Jukcy; => Є d084
&KHcy; => Х d0a5
&KJcy; => Ќ d08c
&Kappa; => Κ ce9a
&Kcedil; => Ķ c4b6
&Kcy; => К d09a
&Kfr; => 𝔎 f09d948e
&Kopf; => 𝕂 f09d9582
&Kscr; => 𝒦 f09d92a6
&LJcy; => Љ d089
&LT; => < 3c
&Lacute; => Ĺ c4b9
&Lambda; => Λ ce9b
&Lang; => ⟪ e29faa
&Laplacetrf; => ℒ e28492
&Larr; => ↞ e2869e
&Lcaron; => Ľ c4bd
&Lcedil; => Ļ c4bb
&Lcy; => Л d09b
&LeftAngleBracket; => ⟨ e29fa8
&LeftArrow; => ← e28690
&LeftArrowBar; => ⇤ e287a4
&LeftArrowRightArrow; => ⇆ e28786
&LeftCeiling; => ⌈ e28c88
&LeftDoubleBracket; => ⟦ e29fa6
&LeftDownTeeVector; => ⥡ e2a5a1
&LeftDownVector; => ⇃ e28783
&LeftDownVectorBar; => ⥙ e2a599
&LeftFloor; => ⌊ e28c8a
&LeftRightArrow; => ↔ e28694
&LeftRightVector; => ⥎ e2a58e
&LeftTee; => ⊣ e28aa3
&LeftTeeArrow; => ↤ e286a4
&LeftTeeVector; => ⥚ e2a59a
&LeftTriangle; => ⊲ e28ab2
&LeftTriangleBar; => ⧏ e2a78f
&LeftTriangleEqual; => ⊴ e28ab4
&LeftUpDownVector; => ⥑ e2a591
&LeftUpTeeVector; => ⥠ e2a5a0
&LeftUpVector; => ↿ e286bf
&LeftUpVectorBar; => ⥘ e2a598
&LeftVector; => ↼ e286bc
&LeftVectorBar; => ⥒ e2a592
&Leftarrow; => ⇐ e28790
&Leftrightarrow; => ⇔ e28794
&LessEqualGreater; => ⋚ e28b9a
&LessFullEqual; => ≦ e289a6
&LessGreater; => ≶ e289b6
&LessLess; => ⪡ e2aaa1
&LessSlantEqual; => ⩽ e2a9bd
&LessTilde; => ≲ e289b2
&Lfr; => 𝔏 f09d948f
&Ll; => ⋘ e28b98
&Lleftarrow; => ⇚ e2879a
&Lmidot; => Ŀ c4bf
&LongLeftArrow; => ⟵ e29fb5
&LongLeftRightArrow; => ⟷ e29fb7
&LongRightArrow; => ⟶ e29fb6
&Longleftarrow; => ⟸ e29fb8
&Longleftrightarrow; => ⟺ e29fba
&Longrightarrow; => ⟹ e29fb9
&Lopf; => 𝕃 f09d9583
&LowerLeftArrow; => ↙ e28699
&LowerRightArrow; => ↘ e28698
&Lscr; => ℒ e28492
&Lsh; => ↰ e286b0
&Lstrok; => Ł c581
&Lt; => ≪ e289aa
&Map; => ⤅ e2a485
&Mcy; => М d09c
&MediumSpace; =>   e2819f
&Mellintrf; => ℳ e284b3
&Mfr; => 𝔐 f09d9490
&MinusPlus; => ∓ e28893
&Mopf; => 𝕄 f09d9584
&Mscr; => ℳ e284b3
&Mu; => Μ ce9c
&NJcy; => Њ d08a
&Nacute; => Ń c583
&Ncaron; => Ň c587
&Ncedil; => Ņ c585
&Ncy; => Н d09d
&NegativeMediumSpace; => ​ e2808b
&NegativeThickSpace; => ​ e2808b
&NegativeThinSpace; => ​ e2808b
&NegativeVeryThinSpace; => ​ e2808b
&NestedGreaterGreater; => ≫ e289ab
&NestedLessLess; => ≪ e289aa
&NewLine; => 
 0a
&Nfr; => 𝔑 f09d9491
&NoBreak; => ⁠ e281a0
&NonBreakingSpace; =>   c2a0
&Nopf; => ℕ e28495
&Not; => ⫬ e2abac
&NotCongruent; => ≢ e289a2
&NotCupCap; => ≭ e289ad
&NotDoubleVerticalBar; => ∦ e288a6
&NotElement; => ∉ e28889
&NotEqual; => ≠ e289a0
&NotEqualTilde; => ≂̸ e28982ccb8
&NotExists; => ∄ e28884
&NotGreater; => ≯ e289af
&NotGreaterEqual; => ≱ e289b1
&NotGreaterFullEqual; => ≧̸ e289a7ccb8
&NotGreaterGreater; => ≫̸ e289abccb8
&NotGreaterLess; => ≹ e289b9
&NotGreaterSlantEqual; => ⩾̸ e2a9beccb8
&NotGreaterTilde; => ≵ e289b5
&NotHumpDownHump; => ≎̸ e2898eccb8
&NotHumpEqual; => ≏̸ e2898fccb8
&NotLeftTriangle; => ⋪ e28baa
&NotLeftTriangleBar; => ⧏̸ e2a78fccb8
&NotLeftTriangleEqual; => ⋬ e28bac
&NotLess; => ≮ e289ae
&NotLessEqual; => ≰ e289b0
&NotLessGreater; => ≸ e289b8
&NotLessLess; => ≪̸ e289aaccb8
&NotLessSlantEqual; => ⩽̸ e2a9bdccb8
&NotLessTilde; => ≴ e289b4
&NotNestedGreaterGreater; => ⪢̸ e2aaa2ccb8
&NotNestedLessLess; => ⪡̸ e2aaa1ccb8
&NotPrecedes; => ⊀ e28a80
&NotPrecedesEqual; => ⪯̸ e2aaafccb8
&NotPrecedesSlantEqual; => ⋠ e28ba0
&NotReverseElement; => ∌ e2888c
&NotRightTriangle; => ⋫ e28bab
&NotRightTriangleBar; => ⧐̸ e2a790ccb8
&NotRightTriangleEqual; => ⋭ e28bad
&NotSquareSubset; => ⊏̸ e28a8fccb8
&NotSquareSubsetEqual; => ⋢ e28ba2
&NotSquareSuperset; => ⊐̸ e28a90ccb8
&NotSquareSupersetEqual; => ⋣ e28ba3
&NotSubset; => ⊂⃒ e28a82e28392
&NotSubsetEqual; => ⊈ e28a88
&NotSucceeds; => ⊁ e28a81
&NotSucceedsEqual; => ⪰̸ e2aab0ccb8
&NotSucceedsSlantEqual; => ⋡ e28ba1
&NotSucceedsTilde; => ≿̸ e289bfccb8
&NotSuperset; => ⊃⃒ e28a83e28392
&NotSupersetEqual; => ⊉ e28a89
&NotTilde; => ≁ e28981
&NotTildeEqual; => ≄ e28984
&NotTildeFullEqual; => ≇ e28987
&NotTildeTilde; => ≉ e28989
&NotVerticalBar; => ∤ e288a4
&Nscr; => 𝒩 f09d92a9
&Ntilde; => Ñ c391
&Nu; => Ν ce9d
&OElig; => Œ c592
&Oacute; => Ó c393
&Ocirc; => Ô c394
&Ocy; => О d09e
&Odblac; => Ő c590
&Ofr; => 𝔒 f09d9492
&Ograve; => Ò c392
&Omacr; => Ō c58c
&Omega; => Ω cea9
&Omicron; => Ο ce9f
&Oopf; => 𝕆 f09d9586
&OpenCurlyDoubleQuote; => “ e2809c
&OpenCurlyQuote; => ‘ e28098
&Or; => ⩔ e2a994
&Oscr; => 𝒪 f09d92aa
&Oslash; => Ø c398
&Otilde; => Õ c395
&Otimes; => ⨷ e2a8b7
&Ouml; => Ö c396
&OverBar; => ‾ e280be
&OverBrace; => ⏞ e28f9e
&OverBracket; => ⎴ e28eb4
&OverParenthesis; => ⏜ e28f9c
&PartialD; => ∂ e28882
&Pcy; => П d09f
&Pfr; => 𝔓 f09d9493
&Phi; => Φ cea6
&Pi; => Π cea0
&PlusMinus; => ± c2b1
&Poincareplane; => ℌ e2848c
&Popf; => ℙ e28499
&Pr; => ⪻ e2aabb
&Precedes; => ≺ e289ba
&PrecedesEqual; => ⪯ e2aaaf
&PrecedesSlantEqual; => ≼ e289bc
&PrecedesTilde; => ≾ e289be
&Prime; => ″ e280b3
&Product; => ∏ e2888f
&Proportion; => ∷ e288b7
&Proportional; => ∝ e2889d
&Pscr; => 𝒫 f09d92ab
&Psi; => Ψ cea8
&QUOT; => " 22
&Qfr; => 𝔔 f09d9494
&Qopf; => ℚ e2849a
&Qscr; => 𝒬 f09d92ac
&RBarr; => ⤐ e2a490
&REG; => ® c2ae
&Racute; => Ŕ c594
&Rang; => ⟫ e29fab
&Rarr; => ↠ e286a0
&Rarrtl; => ⤖ e2a496
&Rcaron; => Ř c598
&Rcedil; => Ŗ c596
&Rcy; => Р d0a0
&Re; => ℜ e2849c
&ReverseElement; => ∋ e2888b
&ReverseEquilibrium; => ⇋ e2878b
&ReverseUpEquilibrium; => ⥯ e2a5af
&Rfr; => ℜ e2849c
&Rho; => Ρ cea1
&RightAngleBracket; => ⟩ e29fa9
&RightArrow; => → e28692
&RightArrowBar; => ⇥ e287a5
&RightArrowLeftArrow; => ⇄ e28784
&RightCeiling; => ⌉ e28c89
&RightDoubleBracket; => ⟧ e29fa7
&RightDownTeeVector; => ⥝ e2a59d
&RightDownVector; => ⇂ e28782
&RightDownVectorBar; => ⥕ e2a595
&RightFloor; => ⌋ e28c8b
&RightTee; => ⊢ e28aa2
&RightTeeArrow; => ↦ e286a6
&RightTeeVector; => ⥛ e2a59b
&RightTriangle; => ⊳ e28ab3
&RightTriangleBar; => ⧐ e2a790
&RightTriangleEqual; => ⊵ e28ab5
&RightUpDownVector; => ⥏ e2a58f
&RightUpTeeVector; => ⥜ e2a59c
&RightUpVector; => ↾ e286be
&RightUpVectorBar; => ⥔ e2a594
&RightVector; => ⇀ e28780
&RightVectorBar; => ⥓ e2a593
&Rightarrow; => ⇒ e28792
&Ropf; => ℝ e2849d
&RoundImplies; => ⥰ e2a5b0
&Rrightarrow; => ⇛ e2879b
&Rscr; => ℛ e2849b
&Rsh; => ↱ e286b1
&RuleDelayed; => ⧴ e2a7b4
&SHCHcy; => Щ d0a9
&SHcy; => Ш d0a8
&SOFTcy; => Ь d0ac
&Sacute; => Ś c59a
&Sc; => ⪼ e2aabc
&Scaron; => Š c5a0
&Scedil; => Ş c59e
&Scirc; => Ŝ c59c
&Scy; => С d0a1
&Sfr; => 𝔖 f09d9496
&ShortDownArrow; => ↓ e28693
&ShortLeftArrow; => ← e28690
&ShortRightArrow; => → e28692
&ShortUpArrow; => ↑ e28691
&Sigma; => Σ cea3
&SmallCircle; => ∘ e28898
&Sopf; => 𝕊 f09d958a
&Sqrt; => √ e2889a
&Square; => □ e296a1
&SquareIntersection; => ⊓ e28a93
&SquareSubset; => ⊏ e28a8f
&SquareSubsetEqual; => ⊑ e28a91
&SquareSuperset; => ⊐ e28a90
&SquareSupersetEqual; => ⊒ e28a92
&SquareUnion; => ⊔ e28a94
&Sscr; => 𝒮 f09d92ae
&Star; => ⋆ e28b86
&Sub; => ⋐ e28b90
&Subset; => ⋐ e28b90
&SubsetEqual; => ⊆ e28a86
&Succeeds; => ≻ e289bb
&SucceedsEqual; => ⪰ e2aab0
&SucceedsSlantEqual; => ≽ e289bd
&SucceedsTilde; => ≿ e289bf
&SuchThat; => ∋ e2888b
&Sum; => ∑ e28891
&Sup; => ⋑ e28b91
&Superset; => ⊃ e28a83
&SupersetEqual; => ⊇ e28a87
&Supset; => ⋑ e28b91
&THORN; => Þ c39e
&TRADE; => ™ e284a2
&TSHcy; => Ћ d08b
&TScy; => Ц d0a6
&Tab; => 	 09
&Tau; => Τ cea4
&Tcaron; => Ť c5a4
&Tcedil; => Ţ c5a2
&Tcy; => Т d0a2
&Tfr; => 𝔗 f09d9497
&Therefore; => ∴ e288b4
&Theta; => Θ ce98
&ThickSpace; =>    e2819fe2808a
&ThinSpace; =>   e28089
&Tilde; => ∼ e288bc
&TildeEqual; => ≃ e28983
&TildeFullEqual; => ≅ e28985
&TildeTilde; => ≈ e28988
&Topf; => 𝕋 f09d958b
&TripleDot; => ⃛ e2839b
&Tscr; => 𝒯 f09d92af
&Tstrok; => Ŧ c5a6
&Uacute; => Ú c39a
&Uarr; => ↟ e2869f
&Uarrocir; => ⥉ e2a589
&Ubrcy; => Ў d08e
&Ubreve; => Ŭ c5ac
&Ucirc; => Û c39b
&Ucy; => У d0a3
&Udblac; => Ű c5b0
&Ufr; => 𝔘 f09d9498
&Ugrave; => Ù c399
&Umacr; => Ū c5aa
&UnderBar; => _ 5f
&UnderBrace; => ⏟ e28f9f
&UnderBracket; => ⎵ e28eb5
&UnderParenthesis; => ⏝ e28f9d
&Union; => ⋃ e28b83
&UnionPlus; => ⊎ e28a8e
&Uogon; => Ų c5b2
&Uopf; => 𝕌 f09d958c
&UpArrow; => ↑ e28691
&UpArrowBar; => ⤒ e2a492
&UpArrowDownArrow; => ⇅ e28785
&UpDownArrow; => ↕ e28695
&UpEquilibrium; => ⥮ e2a5ae
&UpTee; => ⊥ e28aa5
&UpTeeArrow; => ↥ e286a5
&Uparrow; => ⇑ e28791
&Updownarrow; => ⇕ e28795
&UpperLeftArrow; => ↖ e28696
&UpperRightArrow; => ↗ e28697
&Upsi; => ϒ cf92
&Upsilon; => Υ cea5
&Uring; => Ů c5ae
&Uscr; => 𝒰 f09d92b0
&Utilde; => Ũ c5a8
&Uuml; => Ü c39c
&VDash; => ⊫ e28aab
&Vbar; => ⫫ e2abab
&Vcy; => В d092
&Vdash; => ⊩ e28aa9
&Vdashl; => ⫦ e2aba6
&Vee; => ⋁ e28b81
&Verbar; => ‖ e28096
&Vert; => ‖ e28096
&VerticalBar; => ∣ e288a3
&VerticalLine; => | 7c
&VerticalSeparator; => ❘ e29d98
&VerticalTilde; => ≀ e28980
&VeryThinSpace; =>   e2808a
&Vfr; => 𝔙 f09d9499
&Vopf; => 𝕍 f09d958d
&Vscr; => 𝒱 f09d92b1
&Vvdash; => ⊪ e28aaa
&Wcirc; => Ŵ c5b4
&Wedge; => ⋀ e28b80
&Wfr; => 𝔚 f09d949a
&Wopf; => 𝕎 f09d958e
&Wscr; => 𝒲 f09d92b2
&Xfr; => 𝔛 f09d949b
&Xi; => Ξ ce9e
&Xopf; => 𝕏 f09d958f
&Xscr; => 𝒳 f09d92b3
&YAcy; => Я d0af
&YIcy; => Ї d087
&YUcy; => Ю d0ae
&Yacute; => Ý c39d
&Ycirc; => Ŷ c5b6
&Ycy; => Ы d0ab
&Yfr; => 𝔜 f09d949c
&Yopf; => 𝕐 f09d9590
&Yscr; => 𝒴 f09d92b4
&Yuml; => Ÿ c5b8
&ZHcy; => Ж d096
&Zacute; => Ź c5b9
&Zcaron; => Ž c5bd
&Zcy; => З d097
&Zdot; => Ż c5bb
&ZeroWidthSpace; => ​ e2808b
&Zeta; => Ζ ce96
&Zfr; => ℨ e284a8
&Zopf; => ℤ e284a4
&Zscr; => 𝒵 f09d92b5
&aacute; => á c3a1
&abreve; => ă c483
&ac; => ∾ e288be
&acE; => ∾̳ e288beccb3
&acd; => ∿ e288bf
&acirc; => â c3a2
&acute; => ´ c2b4
&acy; => а d0b0
&aelig; => æ c3a6
&af; => ⁡ e281a1
&afr; => 𝔞 f09d949e
&agrave; => à c3a0
&alefsym; => ℵ e284b5
&aleph; => ℵ e284b5
&alpha; => α ceb1
&amacr; => ā c481
&amalg; => ⨿ e2a8bf
&amp; => & 26
&and; => ∧ e288a7
&andand; => ⩕ e2a995
&andd; => ⩜ e2a99c
&andslope; => ⩘ e2a998
&andv; => ⩚ e2a99a
&ang; => ∠ e288a0
&ange; => ⦤ e2a6a4
&angle; => ∠ e288a0
&angmsd; => ∡ e288a1
&angmsdaa; => ⦨ e2a6a8
&angmsdab; => ⦩ e2a6a9
&angmsdac; => ⦪ e2a6aa
&angmsdad; => ⦫ e2a6ab
&angmsdae; => ⦬ e2a6ac
&angmsdaf; => ⦭ e2a6ad
&angmsdag; => ⦮ e2a6ae
&angmsdah; => ⦯ e2a6af
&angrt; => ∟ e2889f
&angrtvb; => ⊾ e28abe
&angrtvbd; => ⦝ e2a69d
&angsph; => ∢ e288a2
&angst; => Å c385
&angzarr; => ⍼ e28dbc
&aogon; => ą c485
&aopf; => 𝕒 f09d9592
&ap; => ≈ e28988
&apE; => ⩰ e2a9b0
&apacir; => ⩯ e2a9af
&ape; => ≊ e2898a
&apid; => ≋ e2898b
&apos; => ' 27
&approx; => ≈ e28988
&approxeq; => ≊ e2898a
&aring; => å c3a5
&ascr; => 𝒶 f09d92b6
&ast; => * 2a
&asymp; => ≈ e28988
&asympeq; => ≍ e2898d
&atilde; => ã c3a3
&auml; => ä c3a4
&awconint; => ∳ e288b3
&awint; => ⨑ e2a891
&bNot; => ⫭ e2abad
&backcong; => ≌ e2898c
&backepsilon; => ϶ cfb6
&backprime; => ‵ e280b5
&backsim; => ∽ e288bd
&backsimeq; => ⋍ e28b8d
&barvee; => ⊽ e28abd
&barwed; => ⌅ e28c85
&barwedge; => ⌅ e28c85
&bbrk; => ⎵ e28eb5
&bbrktbrk; => ⎶ e28eb6
&bcong; => ≌ e2898c
&bcy; => б d0b1
&bdquo; => „ e2809e
&becaus; => ∵ e288b5
&because; => ∵ e288b5
&bemptyv; => ⦰ e2a6b0
&bepsi; => ϶ cfb6
&bernou; => ℬ e284ac
&beta; => β ceb2
&beth; => ℶ e284b6
&between; => ≬ e289ac
&bfr; => 𝔟 f09d949f
&bigcap; => ⋂ e28b82
&bigcirc; => ◯ e297af
&bigcup; => ⋃ e28b83
&bigodot; => ⨀ e2a880
&bigoplus; => ⨁ e2a881
&bigotimes; => ⨂ e2a882
&bigsqcup; => ⨆ e2a886
&bigstar; => ★ e29885
&bigtriangledown; => ▽ e296bd
&bigtriangleup; => △ e296b3
&biguplus; => ⨄ e2a884
&bigvee; => ⋁ e28b81
&bigwedge; => ⋀ e28b80
&bkarow; => ⤍ e2a48d
&blacklozenge; => ⧫ e2a7ab
&blacksquare; => ▪ e296aa
&blacktriangle; => ▴ e296b4
&blacktriangledown; => ▾ e296be
&blacktriangleleft; => ◂ e29782
&blacktriangleright; => ▸ e296b8
&blank; => ␣ e290a3
&blk12; => ▒ e29692
&blk14; => ░ e29691
&blk34; => ▓ e29693
&block; => █ e29688
&bne; => =⃥ 3de283a5
&bnequiv; => ≡⃥ e289a1e283a5
&bnot; => ⌐ e28c90
&bopf; => 𝕓 f09d9593
&bot; => ⊥ e28aa5
&bottom; => ⊥ e28aa5
&bowtie; => ⋈ e28b88
&boxDL; => ╗ e29597
&boxDR; => ╔ e29594
&boxDl; => ╖ e29596
&boxDr; => ╓ e29593
&boxH; => ═ e29590
&boxHD; => ╦ e295a6
&boxHU; => ╩ e295a9
&boxHd; => ╤ e295a4
&boxHu; => ╧ e295a7
&boxUL; => ╝ e2959d
&boxUR; => ╚ e2959a
&boxUl; => ╜ e2959c
&boxUr; => ╙ e29599
&boxV; => ║ e29591
&boxVH; => ╬ e295ac
&boxVL; => ╣ e295a3
&boxVR; => ╠ e295a0
&boxVh; => ╫ e295ab
&boxVl; => ╢ e295a2
&boxVr; => ╟ e2959f
&boxbox; => ⧉ e2a789
&boxdL; => ╕ e29595
&boxdR; => ╒ e29592
&boxdl; => ┐ e29490
&boxdr; => ┌ e2948c
&boxh; => ─ e29480
&boxhD; => ╥ e295a5
&boxhU; => ╨ e295a8
&boxhd; => ┬ e294ac
&boxhu; => ┴ e294b4
&boxminus; => ⊟ e28a9f
&boxplus; => ⊞ e28a9e
&boxtimes; => ⊠ e28aa0
&boxuL; => ╛ e2959b
&boxuR; => ╘ e29598
&boxul; => ┘ e29498
&boxur; => └ e29494
&boxv; => │ e29482
&boxvH; => ╪ e295aa
&boxvL; => ╡ e295a1
&boxvR; => ╞ e2959e
&boxvh; => ┼ e294bc
&boxvl; => ┤ e294a4
&boxvr; => ├ e2949c
&bprime; => ‵ e280b5
&breve; => ˘ cb98
&brvbar; => ¦ c2a6
&bscr; => 𝒷 f09d92b7
&bsemi; => ⁏ e2818f
&bsim; => ∽ e288bd
&bsime; => ⋍ e28b8d
&bsol; => \ 5c
&bsolb; => ⧅ e2a785
&bsolhsub; => ⟈ e29f88
&bull; => • e280a2
&bullet; => • e280a2
&bump; => ≎ e2898e
&bumpE; => ⪮ e2aaae
&bumpe; => ≏ e2898f
&bumpeq; => ≏ e2898f
&cacute; => ć c487
&cap; => ∩ e288a9
&capand; => ⩄ e2a984
&capbrcup; => ⩉ e2a989
&capcap; => ⩋ e2a98b
&capcup; => ⩇ e2a987
&capdot; => ⩀ e2a980
&caps; => ∩︀ e288a9efb880
&caret; => ⁁ e28181
&caron; => ˇ cb87
&ccaps; => ⩍ e2a98d
&ccaron; => č c48d
&ccedil; => ç c3a7
&ccirc; => ĉ c489
&ccups; => ⩌ e2a98c
&ccupssm; => ⩐ e2a990
&cdot; => ċ c48b
&cedil; => ¸ c2b8
&cemptyv; => ⦲ e2a6b2
&cent; => ¢ c2a2
&centerdot; => · c2b7
&cfr; => 𝔠 f09d94a0
&chcy; => ч d187
&check; => ✓ e29c93
&checkmark; => ✓ e29c93
&chi; => χ cf87
&cir; => ○ e2978b
&cirE; => ⧃ e2a783
&circ; => ˆ cb86
&circeq; => ≗ e28997
&circlearrowleft; => ↺ e286ba
&circlearrowright; => ↻ e286bb
&circledR; => ® c2ae
&circledS; => Ⓢ e29388
&circledast; => ⊛ e28a9b
&circledcirc; => ⊚ e28a9a
&circleddash; => ⊝ e28a9d
&cire; => ≗ e28997
&cirfnint; => ⨐ e2a890
&cirmid; => ⫯ e2abaf
&cirscir; => ⧂ e2a782
&clubs; => ♣ e299a3
&clubsuit; => ♣ e299a3
&colon; => : 3a
&colone; => ≔ e28994
&coloneq; => ≔ e28994
&comma; => , 2c
&commat; => @ 40
&comp; => ∁ e28881
&compfn; => ∘ e28898
&complement; => ∁ e28881
&complexes; => ℂ e28482
&cong; => ≅ e28985
&congdot; => ⩭ e2a9ad
&conint; => ∮ e288ae
&copf; => 𝕔 f09d9594
&coprod; => ∐ e28890
&copy; => © c2a9
&copysr; => ℗ e28497
&crarr; => ↵ e286b5
&cross; => ✗ e29c97
&cscr; => 𝒸 f09d92b8
&csub; => ⫏ e2ab8f
&csube; => ⫑ e2ab91
&csup; => ⫐ e2ab90
&csupe; => ⫒ e2ab92
&ctdot; => ⋯ e28baf
&cudarrl; => ⤸ e2a4b8
&cudarrr; => ⤵ e2a4b5
&cuepr; => ⋞ e28b9e
&cuesc; => ⋟ e28b9f
&cularr; => ↶ e286b6
&cularrp; => ⤽ e2a4bd
&cup; => ∪ e288aa
&cupbrcap; => ⩈ e2a988
&cupcap; => ⩆ e2a986
&cupcup; => ⩊ e2a98a
&cupdot; => ⊍ e28a8d
&cupor; => ⩅ e2a985
&cups; => ∪︀ e288aaefb880
&curarr; => ↷ e286b7
&curarrm; => ⤼ e2a4bc
&curlyeqprec; => ⋞ e28b9e
&curlyeqsucc; => ⋟ e28b9f
&curlyvee; => ⋎ e28b8e
&curlywedge; => ⋏ e28b8f
&curren; => ¤ c2a4
&curvearrowleft; => ↶ e286b6
&curvearrowright; => ↷ e286b7
&cuvee; => ⋎ e28b8e
&cuwed; => ⋏ e28b8f
&cwconint; => ∲ e288b2
&cwint; => ∱ e288b1
&cylcty; => ⌭ e28cad
&dArr; => ⇓ e28793
&dHar; => ⥥ e2a5a5
&dagger; => † e280a0
&daleth; => ℸ e284b8
&darr; => ↓ e28693
&dash; => ‐ e28090
&dashv; => ⊣ e28aa3
&dbkarow; => ⤏ e2a48f
&dblac; => ˝ cb9d
&dcaron; => ď c48f
&dcy; => д d0b4
&dd; => ⅆ e28586
&ddagger; => ‡ e280a1
&ddarr; => ⇊ e2878a
&ddotseq; => ⩷ e2a9b7
&deg; => ° c2b0
&delta; => δ ceb4
&demptyv; => ⦱ e2a6b1
&dfisht; => ⥿ e2a5bf
&dfr; => 𝔡 f09d94a1
&dharl; => ⇃ e28783
&dharr; => ⇂ e28782
&diam; => ⋄ e28b84
&diamond; => ⋄ e28b84
&diamondsuit; => ♦ e299a6
&diams; => ♦ e299a6
&die; => ¨ c2a8
&digamma; => ϝ cf9d
&disin; => ⋲ e28bb2
&div; => ÷ c3b7
&divide; => ÷ c3b7
&divideontimes; => ⋇ e28b87
&divonx; => ⋇ e28b87
&djcy; => ђ d192
&dlcorn; => ⌞ e28c9e
&dlcrop; => ⌍ e28c8d
&dollar; => $ 24
&dopf; => 𝕕 f09d9595
&dot; => ˙ cb99
&doteq; => ≐ e28990
&doteqdot; => ≑ e28991
&dotminus; => ∸ e288b8
&dotplus; => ∔ e28894
&dotsquare; => ⊡ e28aa1
&doublebarwedge; => ⌆ e28c86
&downarrow; => ↓ e28693
&downdownarrows; => ⇊ e2878a
&downharpoonleft; => ⇃ e28783
&downharpoonright; => ⇂ e28782
&drbkarow; => ⤐ e2a490
&drcorn; => ⌟ e28c9f
&drcrop; => ⌌ e28c8c
&dscr; => 𝒹 f09d92b9
&dscy; => ѕ d195
&dsol; => ⧶ e2a7b6
&dstrok; => đ c491
&dtdot; => ⋱ e28bb1
&dtri; => ▿ e296bf
&dtrif; => ▾ e296be
&duarr; => ⇵ e287b5
&duhar; => ⥯ e2a5af
&dwangle; => ⦦ e2a6a6
&dzcy; => џ d19f
&dzigrarr; => ⟿ e29fbf
&eDDot; => ⩷ e2a9b7
&eDot; => ≑ e28991
&eacute; => é c3a9
&easter; => ⩮ e2a9ae
&ecaron; => ě c49b
&ecir; => ≖ e28996
&ecirc; => ê c3aa
&ecolon; => ≕ e28995
&ecy; => э d18d
&edot; => ė c497
&ee; => ⅇ e28587
&efDot; => ≒ e28992
&efr; => 𝔢 f09d94a2
&eg; => ⪚ e2aa9a
&egrave; => è c3a8
&egs; => ⪖ e2aa96
&egsdot; => ⪘ e2aa98
&el; => ⪙ e2aa99
&elinters; => ⏧ e28fa7
&ell; => ℓ e28493
&els; => ⪕ e2aa95
&elsdot; => ⪗ e2aa97
&emacr; => ē c493
&empty; => ∅ e28885
&emptyset; => ∅ e28885
&emptyv; => ∅ e28885
&emsp; =>   e28083
&emsp13; =>   e28084
&emsp14; =>   e28085
&eng; => ŋ c58b
&ensp; =>   e28082
&eogon; => ę c499
&eopf; => 𝕖 f09d9596
&epar; => ⋕ e28b95
&eparsl; => ⧣ e2a7a3
&eplus; => ⩱ e2a9b1
&epsi; => ε ceb5
&epsilon; => ε ceb5
&epsiv; => ϵ cfb5
&eqcirc; => ≖ e28996
&eqcolon; => ≕ e28995
&eqsim; => ≂ e28982
&eqslantgtr; => ⪖ e2aa96
&eqslantless; => ⪕ e2aa95
&equals; => = 3d
&equest; => ≟ e2899f
&equiv; => ≡ e289a1
&equivDD; => ⩸ e2a9b8
&eqvparsl; => ⧥ e2a7a5
&erDot; => ≓ e28993
&erarr; => ⥱ e2a5b1
&escr; => ℯ e284af
&esdot; => ≐ e28990
&esim; => ≂ e28982
&eta; => η ceb7
&eth; => ð c3b0
&euml; => ë c3ab
&euro; => € e282ac
&excl; => ! 21
&exist; => ∃ e28883
&expectation; => ℰ e284b0
&exponentiale; => ⅇ e28587
&fallingdotseq; => ≒ e28992
&fcy; => ф d184
&female; => ♀ e29980
&ffilig; => ﬃ efac83
&fflig; => ﬀ efac80
&ffllig; => ﬄ efac84
&ffr; => 𝔣 f09d94a3
&filig; => ﬁ efac81
&fjlig; => fj 666a
&flat; => ♭ e299ad
&fllig; => ﬂ efac82
&fltns; => ▱ e296b1
&fnof; => ƒ c692
&fopf; => 𝕗 f09d9597
&forall; => ∀ e28880
&fork; => ⋔ e28b94
&forkv; => ⫙ e2ab99
&fpartint; => ⨍ e2a88d
&frac12; => ½ c2bd
&frac13; => ⅓ e28593
&frac14; => ¼ c2bc
&frac15; => ⅕ e28595
&frac16; => ⅙ e28599
&frac18; => ⅛ e2859b
&frac23; => ⅔ e28594
&frac25; => ⅖ e28596
&frac34; => ¾ c2be
&frac35; => ⅗ e28597
&frac38; => ⅜ e2859c
&frac45; => ⅘ e28598
&frac56; => ⅚ e2859a
&frac58; => ⅝ e2859d
&frac78; => ⅞ e2859e
&frasl; => ⁄ e28184
&frown; => ⌢ e28ca2
&fscr; => 𝒻 f09d92bb
&gE; => ≧ e289a7
&gEl; => ⪌ e2aa8c
&gacute; => ǵ c7b5
&gamma; => γ ceb3
&gammad; => ϝ cf9d
&gap; => ⪆ e2aa86
&gbreve; => ğ c49f
&gcirc; => ĝ c49d
&gcy; => г d0b3
&gdot; => ġ c4a1
&ge; => ≥ e289a5
&gel; => ⋛ e28b9b
&geq; => ≥ e289a5
&geqq; => ≧ e289a7
&geqslant; => ⩾ e2a9be
&ges; => ⩾ e2a9be
&gescc; => ⪩ e2aaa9
&gesdot; => ⪀ e2aa80
&gesdoto; => ⪂ e2aa82
&gesdotol; => ⪄ e2aa84
&gesl; => ⋛︀ e28b9befb880
&gesles; => ⪔ e2aa94
&gfr; => 𝔤 f09d94a4
&gg; => ≫ e289ab
&ggg; => ⋙ e28b99
&gimel; => ℷ e284b7
&gjcy; => ѓ d193
&gl; => ≷ e289b7
&glE; => ⪒ e2aa92
&gla; => ⪥ e2aaa5
&glj; => ⪤ e2aaa4
&gnE; => ≩ e289a9
&gnap; => ⪊ e2aa8a
&gnapprox; => ⪊ e2aa8a
&gne; => ⪈ e2aa88
&gneq; => ⪈ e2aa88
&gneqq; => ≩ e289a9
&gnsim; => ⋧ e28ba7
&gopf; => 𝕘 f09d9598
&grave; => ` 60
&gscr; => ℊ e2848a
&gsim; => ≳ e289b3
&gsime; => ⪎ e2aa8e
&gsiml; => ⪐ e2aa90
&gt; => > 3e
&gtcc; => ⪧ e2aaa7
&gtcir; => ⩺ e2a9ba
&gtdot; => ⋗ e28b97
&gtlPar; => ⦕ e2a695
&gtquest; => ⩼ e2a9bc
&gtrapprox; => ⪆ e2aa86
&gtrarr; => ⥸ e2a5b8
&gtrdot; => ⋗ e28b97
&gtreqless; => ⋛ e28b9b
&gtreqqless; => ⪌ e2aa8c
&gtrless; => ≷ e289b7
&gtrsim; => ≳ e289b3
&gvertneqq; => ≩︀ e289a9efb880
&gvnE; => ≩︀ e289a9efb880
&hArr; => ⇔ e28794
&hairsp; =>   e2808a
&half; => ½ c2bd
&hamilt; => ℋ e2848b
&hardcy; => ъ d18a
&harr; => ↔ e28694
&harrcir; => ⥈ e2a588
&harrw; => ↭ e286ad
&hbar; => ℏ e2848f
&hcirc; => ĥ c4a5
&hearts; => ♥ e299a5
&heartsuit; => ♥ e299a5
&hellip; => … e280a6
&hercon; => ⊹ e28ab9
&hfr; => 𝔥 f09d94a5
&hksearow; => ⤥ e2a4a5
&hkswarow; => ⤦ e2a4a6
&hoarr; => ⇿ e287bf
&homtht; => ∻ e288bb
&hookleftarrow; => ↩ e286a9
&hookrightarrow; => ↪ e286aa
&hopf; => 𝕙 f09d9599
&horbar; => ― e28095
&hscr; => 𝒽 f09d92bd
&hslash; => ℏ e2848f
&hstrok; => ħ c4a7
&hybull; => ⁃ e28183
&hyphen; => ‐ e28090
&iacute; => í c3ad
&ic; => ⁣ e281a3
&icirc; => î c3ae
&icy; => и d0b8
&iecy; => е d0b5
&iexcl; => ¡ c2a1
&iff; => ⇔ e28794
&ifr; => 𝔦 f09d94a6
&igrave; => ì c3ac
&ii; => ⅈ e28588
&iiiint; => ⨌ e2a88c
&iiint; => ∭ e288ad
&iinfin; => ⧜ e2a79c
&iiota; => ℩ e284a9
&ijlig; => ĳ c4b3
&imacr; => ī c4ab
&image; => ℑ e28491
&imagline; => ℐ e28490
&imagpart; => ℑ e28491
&imath; => ı c4b1
&imof; => ⊷ e28ab7
&imped; => Ƶ c6b5
&in; => ∈ e28888
&incare; => ℅ e28485
&infin; => ∞ e2889e
&infintie; => ⧝ e2a79d
&inodot; => ı c4b1
&int; => ∫ e288ab
&intcal; => ⊺ e28aba
&integers; => ℤ e284a4
&intercal; => ⊺ e28aba
&intlarhk; => ⨗ e2a897
&intprod; => ⨼ e2a8bc
&iocy; => ё d191
&iogon; => į c4af
&iopf; => 𝕚 f09d959a
&iota; => ι ceb9
&iprod; => ⨼ e2a8bc
&iquest; => ¿ c2bf
&iscr; => 𝒾 f09d92be
&isin; => ∈ e28888
&isinE; => ⋹ e28bb9
&isindot; => ⋵ e28bb5
&isins; => ⋴ e28bb4
&isinsv; => ⋳ e28bb3
&isinv; => ∈ e28888
&it; => ⁢ e281a2
&itilde; => ĩ c4a9
&iukcy; => і d196
&iuml; => ï c3af
&jcirc; => ĵ c4b5
&jcy; => й d0b9
&jfr; => 𝔧 f09d94a7
&jmath; => ȷ c8b7
&jopf; => 𝕛 f09d959b
&jscr; => 𝒿 f09d92bf
&jsercy; => ј d198
&jukcy; => є d194
&kappa; => κ ceba
&kappav; => ϰ cfb0
&kcedil; => ķ c4b7
&kcy; => к d0ba
&kfr; => 𝔨 f09d94a8
&kgreen; => ĸ c4b8
&khcy; => х d185
&kjcy; => ќ d19c
&kopf; => 𝕜 f09d959c
&kscr; => 𝓀 f09d9380
&lAarr; => ⇚ e2879a
&lArr; => ⇐ e28790
&lAtail; => ⤛ e2a49b
&lBarr; => ⤎ e2a48e
&lE; => ≦ e289a6
&lEg; => ⪋ e2aa8b
&lHar; => ⥢ e2a5a2
&lacute; => ĺ c4ba
&laemptyv; => ⦴ e2a6b4
&lagran; => ℒ e28492
&lambda; => λ cebb
&lang; => ⟨ e29fa8
&langd; => ⦑ e2a691
&langle; => ⟨ e29fa8
&lap; => ⪅ e2aa85
&laquo; => « c2ab
&larr; => ← e28690
&larrb; => ⇤ e287a4
&larrbfs; => ⤟ e2a49f
&larrfs; => ⤝ e2a49d
&larrhk; => ↩ e286a9
&larrlp; => ↫ e286ab
&larrpl; => ⤹ e2a4b9
&larrsim; => ⥳ e2a5b3
&larrtl; => ↢ e286a2
&lat; => ⪫ e2aaab
&latail; => ⤙ e2a499
&late; => ⪭ e2aaad
&lates; => ⪭︀ e2aaadefb880
&lbarr; => ⤌ e2a48c
&lbbrk; => ❲ e29db2
&lbrace; => { 7b
&lbrack; => [ 5b
&lbrke; => ⦋ e2a68b
&lbrksld; => ⦏ e2a68f
&lbrkslu; => ⦍ e2a68d
&lcaron; => ľ c4be
&lcedil; => ļ c4bc
&lceil; => ⌈ e28c88
&lcub; => { 7b
&lcy; => л d0bb
&ldca; => ⤶ e2a4b6
&ldquo; => “ e2809c
&ldquor; => „ e2809e
&ldrdhar; => ⥧ e2a5a7
&ldrushar; => ⥋ e2a58b
&ldsh; => ↲ e286b2
&le; => ≤ e289a4
&leftarrow; => ← e28690
&leftarrowtail; => ↢ e286a2
&leftharpoondown; => ↽ e286bd
&leftharpoonup; => ↼ e286bc
&leftleftarrows; => ⇇ e28787
&leftrightarrow; => ↔ e28694
&leftrightarrows; => ⇆ e28786
&leftrightharpoons; => ⇋ e2878b
&leftrightsquigarrow; => ↭ e286ad
&leftthreetimes; => ⋋ e28b8b
&leg; => ⋚ e28b9a
&leq; => ≤ e289a4
&leqq; => ≦ e289a6
&leqslant; => ⩽ e2a9bd
&les; => ⩽ e2a9bd
&lescc; => ⪨ e2aaa8
&lesdot; => ⩿ e2a9bf
&lesdoto; => ⪁ e2aa81
&lesdotor; => ⪃ e2aa83
&lesg; => ⋚︀ e28b9aefb880
&lesges; => ⪓ e2aa93
&lessapprox; => ⪅ e2aa85
&lessdot; => ⋖ e28b96
&lesseqgtr; => ⋚ e28b9a
&lesseqqgtr; => ⪋ e2aa8b
&lessgtr; => ≶ e289b6
&lesssim; => ≲ e289b2
&lfisht; => ⥼ e2a5bc
&lfloor; => ⌊ e28c8a
&lfr; => 𝔩 f09d94a9
&lg; => ≶ e289b6
&lgE; => ⪑ e2aa91
&lhard; => ↽ e286bd
&lharu; => ↼ e286bc
&lharul; => ⥪ e2a5aa
&lhblk; => ▄ e29684
&ljcy; => љ d199
&ll; => ≪ e289aa
&llarr; => ⇇ e28787
&llcorner; => ⌞ e28c9e
&llhard; => ⥫ e2a5ab
&lltri; => ◺ e297ba
&lmidot; => ŀ c580
&lmoust; => ⎰ e28eb0
&lmoustache; => ⎰ e28eb0
&lnE; => ≨ e289a8
&lnap; => ⪉ e2aa89
&lnapprox; => ⪉ e2aa89
&lne; => ⪇ e2aa87
&lneq; => ⪇ e2aa87
&lneqq; => ≨ e289a8
&lnsim; => ⋦ e28ba6
&loang; => ⟬ e29fac
&loarr; => ⇽ e287bd
&lobrk; => ⟦ e29fa6
&longleftarrow; => ⟵ e29fb5
&longleftrightarrow; => ⟷ e29fb7
&longmapsto; => ⟼ e29fbc
&longrightarrow; => ⟶ e29fb6
&looparrowleft; => ↫ e286ab
&looparrowright; => ↬ e286ac
&lopar; => ⦅ e2a685
&lopf; => 𝕝 f09d959d
&loplus; => ⨭ e2a8ad
&lotimes; => ⨴ e2a8b4
&lowast; => ∗ e28897
&lowbar; => _ 5f
&loz; => ◊ e2978a
&lozenge; => ◊ e2978a
&lozf; => ⧫ e2a7ab
&lpar; => ( 28
&lparlt; => ⦓ e2a693
&lrarr; => ⇆ e28786
&lrcorner; => ⌟ e28c9f
&lrhar; => ⇋ e2878b
&lrhard; => ⥭ e2a5ad
&lrm; => ‎ e2808e
&lrtri; => ⊿ e28abf
&lsaquo; => ‹ e280b9
&lscr; => 𝓁 f09d9381
&lsh; => ↰ e286b0
&lsim; => ≲ e289b2
&lsime; => ⪍ e2aa8d
&lsimg; => ⪏ e2aa8f
&lsqb; => [ 5b
&lsquo; => ‘ e28098
&lsquor; => ‚ e2809a
&lstrok; => ł c582
&lt; => < 3c
&ltcc; => ⪦ e2aaa6
&ltcir; => ⩹ e2a9b9
&ltdot; => ⋖ e28b96
&lthree; => ⋋ e28b8b
&ltimes; => ⋉ e28b89
&ltlarr; => ⥶ e2a5b6
&ltquest; => ⩻ e2a9bb
&ltrPar; => ⦖ e2a696
&ltri; => ◃ e29783
&ltrie; => ⊴ e28ab4
&ltrif; => ◂ e29782
&lurdshar; => ⥊ e2a58a
&luruhar; => ⥦ e2a5a6
&lvertneqq; => ≨︀ e289a8efb880
&lvnE; => ≨︀ e289a8efb880
&mDDot; => ∺ e288ba
&macr; => ¯ c2af
&male; => ♂ e29982
&malt; => ✠ e29ca0
&maltese; => ✠ e29ca0
&map; => ↦ e286a6
&mapsto; => ↦ e286a6
&mapstodown; => ↧ e286a7
&mapstoleft; => ↤ e286a4
&mapstoup; => ↥ e286a5
&marker; => ▮ e296ae
&mcomma; => ⨩ e2a8a9
&mcy; => м d0bc
&mdash; => — e28094
&measuredangle; => ∡ e288a1
&mfr; => 𝔪 f09d94aa
&mho; => ℧ e284a7
&micro; => µ c2b5
&mid; => ∣ e288a3
&midast; => * 2a
&midcir; => ⫰ e2abb0
&middot; => · c2b7
&minus; => − e28892
&minusb; => ⊟ e28a9f
&minusd; => ∸ e288b8
&minusdu; => ⨪ e2a8aa
&mlcp; => ⫛ e2ab9b
&mldr; => … e280a6
&mnplus; => ∓ e28893
&models; => ⊧ e28aa7
&mopf; => 𝕞 f09d959e
&mp; => ∓ e28893
&mscr; => 𝓂 f09d9382
&mstpos; => ∾ e288be
&mu; => μ cebc
&multimap; => ⊸ e28ab8
&mumap; => ⊸ e28ab8
&nGg; => ⋙̸ e28b99ccb8
&nGt; => ≫⃒ e289abe28392
&nGtv; => ≫̸ e289abccb8
&nLeftarrow; => ⇍ e2878d
&nLeftrightarrow; => ⇎ e2878e
&nLl; => ⋘̸ e28b98ccb8
&nLt; => ≪⃒ e289aae28392
&nLtv; => ≪̸ e289aaccb8
&nRightarrow; => ⇏ e2878f
&nVDash; => ⊯ e28aaf
&nVdash; => ⊮ e28aae
&nabla; => ∇ e28887
&nacute; => ń c584
&nang; => ∠⃒ e288a0e28392
&nap; => ≉ e28989
&napE; => ⩰̸ e2a9b0ccb8
&napid; => ≋̸ e2898bccb8
&napos; => ŉ c589
&napprox; => ≉ e28989
&natur; => ♮ e299ae
&natural; => ♮ e299ae
&naturals; => ℕ e28495
&nbsp; =>   c2a0
&nbump; => ≎̸ e2898eccb8
&nbumpe; => ≏̸ e2898fccb8
&ncap; => ⩃ e2a983
&ncaron; => ň c588
&ncedil; => ņ c586
&ncong; => ≇ e28987
&ncongdot; => ⩭̸ e2a9adccb8
&ncup; => ⩂ e2a982
&ncy; => н d0bd
&ndash; => – e28093
&ne; => ≠ e289a0
&neArr; => ⇗ e28797
&nearhk; => ⤤ e2a4a4
&nearr; => ↗ e28697
&nearrow; => ↗ e28697
&nedot; => ≐̸ e28990ccb8
&nequiv; => ≢ e289a2
&nesear; => ⤨ e2a4a8
&nesim; => ≂̸ e28982ccb8
&nexist; => ∄ e28884
&nexists; => ∄ e28884
&nfr; => 𝔫 f09d94ab
&ngE; => ≧̸ e289a7ccb8
&nge; => ≱ e289b1
&ngeq; => ≱ e289b1
&ngeqq; => ≧̸ e289a7ccb8
&ngeqslant; => ⩾̸ e2a9beccb8
&nges; => ⩾̸ e2a9beccb8
&ngsim; => ≵ e289b5
&ngt; => ≯ e289af
&ngtr; => ≯ e289af
&nhArr; => ⇎ e2878e
&nharr; => ↮ e286ae
&nhpar; => ⫲ e2abb2
&ni; => ∋ e2888b
&nis; => ⋼ e28bbc
&nisd; => ⋺ e28bba
&niv; => ∋ e2888b
&njcy; => њ d19a
&nlArr; => ⇍ e2878d
&nlE; => ≦̸ e289a6ccb8
&nlarr; => ↚ e2869a
&nldr; => ‥ e280a5
&nle; => ≰ e289b0
&nleftarrow; => ↚ e2869a
&nleftrightarrow; => ↮ e286ae
&nleq; => ≰ e289b0
&nleqq; => ≦̸ e289a6ccb8
&nleqslant; => ⩽̸ e2a9bdccb8
&nles; => ⩽̸ e2a9bdccb8
&nless; => ≮ e289ae
&nlsim; => ≴ e289b4
&nlt; => ≮ e289ae
&nltri; => ⋪ e28baa
&nltrie; => ⋬ e28bac
&nmid; => ∤ e288a4
&nopf; => 𝕟 f09d959f
&not; => ¬ c2ac
&notin; => ∉ e28889
&notinE; => ⋹̸ e28bb9ccb8
&notindot; => ⋵̸ e28bb5ccb8
&notinva; => ∉ e28889
&notinvb; => ⋷ e28bb7
&notinvc; => ⋶ e28bb6
&notni; => ∌ e2888c
&notniva; => ∌ e2888c
&notnivb; => ⋾ e28bbe
&notnivc; => ⋽ e28bbd
&npar; => ∦ e288a6
&nparallel; => ∦ e288a6
&nparsl; => ⫽⃥ e2abbde283a5
&npart; => ∂̸ e28882ccb8
&npolint; => ⨔ e2a894
&npr; => ⊀ e28a80
&nprcue; => ⋠ e28ba0
&npre; => ⪯̸ e2aaafccb8
&nprec; => ⊀ e28a80
&npreceq; => ⪯̸ e2aaafccb8
&nrArr; => ⇏ e2878f
&nrarr; => ↛ e2869b
&nrarrc; => ⤳̸ e2a4b3ccb8
&nrarrw; => ↝̸ e2869dccb8
&nrightarrow; => ↛ e2869b
&nrtri; => ⋫ e28bab
&nrtrie; => ⋭ e28bad
&nsc; => ⊁ e28a81
&nsccue; => ⋡ e28ba1
&nsce; => ⪰̸ e2aab0ccb8
&nscr; => 𝓃 f09d9383
&nshortmid; => ∤ e288a4
&nshortparallel; => ∦ e288a6
&nsim; => ≁ e28981
&nsime; => ≄ e28984
&nsimeq; => ≄ e28984
&nsmid; => ∤ e288a4
&nspar; => ∦ e288a6
&nsqsube; => ⋢ e28ba2
&nsqsupe; => ⋣ e28ba3
&nsub; => ⊄ e28a84
&nsubE; => ⫅̸ e2ab85ccb8
&nsube; => ⊈ e28a88
&nsubset; => ⊂⃒ e28a82e28392
&nsubseteq; => ⊈ e28a88
&nsubseteqq; => ⫅̸ e2ab85ccb8
&nsucc; => ⊁ e28a81
&nsucceq; => ⪰̸ e2aab0ccb8
&nsup; => ⊅ e28a85
&nsupE; => ⫆̸ e2ab86ccb8
&nsupe; => ⊉ e28a89
&nsupset; => ⊃⃒ e28a83e28392
&nsupseteq; => ⊉ e28a89
&nsupseteqq; => ⫆̸ e2ab86ccb8
&ntgl; => ≹ e289b9
&ntilde; => ñ c3b1
&ntlg; => ≸ e289b8
&ntriangleleft; => ⋪ e28baa
&ntrianglelefteq; => ⋬ e28bac
&ntriangleright; => ⋫ e28bab
&ntrianglerighteq; => ⋭ e28bad
&nu; => ν cebd
&num; => # 23
&numero; => № e28496
&numsp; =>   e28087
&nvDash; => ⊭ e28aad
&nvHarr; => ⤄ e2a484
&nvap; => ≍⃒ e2898de28392
&nvdash; => ⊬ e28aac
&nvge; => ≥⃒ e289a5e28392
&nvgt; => >⃒ 3ee28392
&nvinfin; => ⧞ e2a79e
&nvlArr; => ⤂ e2a482
&nvle; => ≤⃒ e289a4e28392
&nvlt; => <⃒ 3ce28392
&nvltrie; => ⊴⃒ e28ab4e28392
&nvrArr; => ⤃ e2a483
&nvrtrie; => ⊵⃒ e28ab5e28392
&nvsim; => ∼⃒ e288bce28392
&nwArr; => ⇖ e28796
&nwarhk; => ⤣ e2a4a3
&nwarr; => ↖ e28696
&nwarrow; => ↖ e28696
&nwnear; => ⤧ e2a4a7
&oS; => Ⓢ e29388
&oacute; => ó c3b3
&oast; => ⊛ e28a9b
&ocir; => ⊚ e28a9a
&ocirc; => ô c3b4
&ocy; => о d0be
&odash; => ⊝ e28a9d
&odblac; => ő c591
&odiv; => ⨸ e2a8b8
&odot; => ⊙ e28a99
&odsold; => ⦼ e2a6bc
&oelig; => œ c593
&ofcir; => ⦿ e2a6bf
&ofr; => 𝔬 f09d94ac
&ogon; => ˛ cb9b
&ograve; => ò c3b2
&ogt; => ⧁ e2a781
&ohbar; => ⦵ e2a6b5
&ohm; => Ω cea9
&oint; => ∮ e288ae
&olarr; => ↺ e286ba
&olcir; => ⦾ e2a6be
&olcross; => ⦻ e2a6bb
&oline; => ‾ e280be
&olt; => ⧀ e2a780
&omacr; => ō c58d
&omega; => ω cf89
&omicron; => ο cebf
&omid; => ⦶ e2a6b6
&ominus; => ⊖ e28a96
&oopf; => 𝕠 f09d95a0
&opar; => ⦷ e2a6b7
&operp; => ⦹ e2a6b9
&oplus; => ⊕ e28a95
&or; => ∨ e288a8
&orarr; => ↻ e286bb
&ord; => ⩝ e2a99d
&order; => ℴ e284b4
&orderof; => ℴ e284b4
&ordf; => ª c2aa
&ordm; => º c2ba
&origof; => ⊶ e28ab6
&oror; => ⩖ e2a996
&orslope; => ⩗ e2a997
&orv; => ⩛ e2a99b
&oscr; => ℴ e284b4
&oslash; => ø c3b8
&osol; => ⊘ e28a98
&otilde; => õ c3b5
&otimes; => ⊗ e28a97
&otimesas; => ⨶ e2a8b6
&ouml; => ö c3b6
&ovbar; => ⌽ e28cbd
&par; => ∥ e288a5
&para; => ¶ c2b6
&parallel; => ∥ e288a5
&parsim; => ⫳ e2abb3
&parsl; => ⫽ e2abbd
&part; => ∂ e28882
&pcy; => п d0bf
&percnt; => % 25
&period; => . 2e
&permil; => ‰ e280b0
&perp; => ⊥ e28aa5
&pertenk; => ‱ e280b1
&pfr; => 𝔭 f09d94ad
&phi; => φ cf86
&phiv; => ϕ cf95
&phmmat; => ℳ e284b3
&phone; => ☎ e2988e
&pi; => π cf80
&pitchfork; => ⋔ e28b94
&piv; => ϖ cf96
&planck; => ℏ e2848f
&planckh; => ℎ e2848e
&plankv; => ℏ e2848f
&plus; => + 2b
&plusacir; => ⨣ e2a8a3
&plusb; => ⊞ e28a9e
&pluscir; => ⨢ e2a8a2
&plusdo; => ∔ e28894
&plusdu; => ⨥ e2a8a5
&pluse; => ⩲ e2a9b2
&plusmn; => ± c2b1
&plussim; => ⨦ e2a8a6
&plustwo; => ⨧ e2a8a7
&pm; => ± c2b1
&pointint; => ⨕ e2a895
&popf; => 𝕡 f09d95a1
&pound; => £ c2a3
&pr; => ≺ e289ba
&prE; => ⪳ e2aab3
&prap; => ⪷ e2aab7
&prcue; => ≼ e289bc
&pre; => ⪯ e2aaaf
&prec; => ≺ e289ba
&precapprox; => ⪷ e2aab7
&preccurlyeq; => ≼ e289bc
&preceq; => ⪯ e2aaaf
&precnapprox; => ⪹ e2aab9
&precneqq; => ⪵ e2aab5
&precnsim; => ⋨ e28ba8
&precsim; => ≾ e289be
&prime; => ′ e280b2
&primes; => ℙ e28499
&prnE; => ⪵ e2aab5
&prnap; => ⪹ e2aab9
&prnsim; => ⋨ e28ba8
&prod; => ∏ e2888f
&profalar; => ⌮ e28cae
&profline; => ⌒ e28c92
&profsurf; => ⌓ e28c93
&prop; => ∝ e2889d
&propto; => ∝ e2889d
&prsim; => ≾ e289be
&prurel; => ⊰ e28ab0
&pscr; => 𝓅 f09d9385
&psi; => ψ cf88
&puncsp; =>   e28088
&qfr; => 𝔮 f09d94ae
&qint; => ⨌ e2a88c
&qopf; => 𝕢 f09d95a2
&qprime; => ⁗ e28197
&qscr; => 𝓆 f09d9386
&quaternions; => ℍ e2848d
&quatint; => ⨖ e2a896
&quest; => ? 3f
&questeq; => ≟ e2899f
&quot; => " 22
&rAarr; => ⇛ e2879b
&rArr; => ⇒ e28792
&rAtail; => ⤜ e2a49c
&rBarr; => ⤏ e2a48f
&rHar; => ⥤ e2a5a4
&race; => ∽̱ e288bdccb1
&racute; => ŕ c595
&radic; => √ e2889a
&raemptyv; => ⦳ e2a6b3
&rang; => ⟩ e29fa9
&rangd; => ⦒ e2a692
&range; => ⦥ e2a6a5
&rangle; => ⟩ e29fa9
&raquo; => » c2bb
&rarr; => → e28692
&rarrap; => ⥵ e2a5b5
&rarrb; => ⇥ e287a5
&rarrbfs; => ⤠ e2a4a0
&rarrc; => ⤳ e2a4b3
&rarrfs; => ⤞ e2a49e
&rarrhk; => ↪ e286aa
&rarrlp; => ↬ e286ac
&rarrpl; => ⥅ e2a585
&rarrsim; => ⥴ e2a5b4
&rarrtl; => ↣ e286a3
&rarrw; => ↝ e2869d
&ratail; => ⤚ e2a49a
&ratio; => ∶ e288b6
&rationals; => ℚ e2849a
&rbarr; => ⤍ e2a48d
&rbbrk; => ❳ e29db3
&rbrace; => } 7d
&rbrack; => ] 5d
&rbrke; => ⦌ e2a68c
&rbrksld; => ⦎ e2a68e
&rbrkslu; => ⦐ e2a690
&rcaron; => ř c599
&rcedil; => ŗ c597
&rceil; => ⌉ e28c89
&rcub; => } 7d
&rcy; => р d180
&rdca; => ⤷ e2a4b7
&rdldhar; => ⥩ e2a5a9
&rdquo; => ” e2809d
&rdquor; => ” e2809d
&rdsh; => ↳ e286b3
&real; => ℜ e2849c
&realine; => ℛ e2849b
&realpart; => ℜ e2849c
&reals; => ℝ e2849d
&rect; => ▭ e296ad
&reg; => ® c2ae
&rfisht; => ⥽ e2a5bd
&rfloor; => ⌋ e28c8b
&rfr; => 𝔯 f09d94af
&rhard; => ⇁ e28781
&rharu; => ⇀ e28780
&rharul; => ⥬ e2a5ac
&rho; => ρ cf81
&rhov; => ϱ cfb1
&rightarrow; => → e28692
&rightarrowtail; => ↣ e286a3
&rightharpoondown; => ⇁ e28781
&rightharpoonup; => ⇀ e28780
&rightleftarrows; => ⇄ e28784
&rightleftharpoons; => ⇌ e2878c
&rightrightarrows; => ⇉ e28789
&rightsquigarrow; => ↝ e2869d
&rightthreetimes; => ⋌ e28b8c
&ring; => ˚ cb9a
&risingdotseq; => ≓ e28993
&rlarr; => ⇄ e28784
&rlhar; => ⇌ e2878c
&rlm; => ‏ e2808f
&rmoust; => ⎱ e28eb1
&rmoustache; => ⎱ e28eb1
&rnmid; => ⫮ e2abae
&roang; => ⟭ e29fad
&roarr; => ⇾ e287be
&robrk; => ⟧ e29fa7
&ropar; => ⦆ e2a686
&ropf; => 𝕣 f09d95a3
&roplus; => ⨮ e2a8ae
&rotimes; => ⨵ e2a8b5
&rpar; => ) 29
&rpargt; => ⦔ e2a694
&rppolint; => ⨒ e2a892
&rrarr; => ⇉ e28789
&rsaquo; => › e280ba
&rscr; => 𝓇 f09d9387
&rsh; => ↱ e286b1
&rsqb; => ] 5d
&rsquo; => ’ e28099
&rsquor; => ’ e28099
&rthree; => ⋌ e28b8c
&rtimes; => ⋊ e28b8a
&rtri; => ▹ e296b9
&rtrie; => ⊵ e28ab5
&rtrif; => ▸ e296b8
&rtriltri; => ⧎ e2a78e
&ruluhar; => ⥨ e2a5a8
&rx; => ℞ e2849e
&sacute; => ś c59b
&sbquo; => ‚ e2809a
&sc; => ≻ e289bb
&scE; => ⪴ e2aab4
&scap; => ⪸ e2aab8
&scaron; => š c5a1
&sccue; => ≽ e289bd
&sce; => ⪰ e2aab0
&scedil; => ş c59f
&scirc; => ŝ c59d
&scnE; => ⪶ e2aab6
&scnap; => ⪺ e2aaba
&scnsim; => ⋩ e28ba9
&scpolint; => ⨓ e2a893
&scsim; => ≿ e289bf
&scy; => с d181
&sdot; => ⋅ e28b85
&sdotb; => ⊡ e28aa1
&sdote; => ⩦ e2a9a6
&seArr; => ⇘ e28798
&searhk; => ⤥ e2a4a5
&searr; => ↘ e28698
&searrow; => ↘ e28698
&sect; => § c2a7
&semi; => ; 3b
&seswar; => ⤩ e2a4a9
&setminus; => ∖ e28896
&setmn; => ∖ e28896
&sext; => ✶ e29cb6
&sfr; => 𝔰 f09d94b0
&sfrown; => ⌢ e28ca2
&sharp; => ♯ e299af
&shchcy; => щ d189
&shcy; => ш d188
&shortmid; => ∣ e288a3
&shortparallel; => ∥ e288a5
&shy; => ­ c2ad
&sigma; => σ cf83
&sigmaf; => ς cf82
&sigmav; => ς cf82
&sim; => ∼ e288bc
&simdot; => ⩪ e2a9aa
&sime; => ≃ e28983
&simeq; => ≃ e28983
&simg; => ⪞ e2aa9e
&simgE; => ⪠ e2aaa0
&siml; => ⪝ e2aa9d
&simlE; => ⪟ e2aa9f
&simne; => ≆ e28986
&simplus; => ⨤ e2a8a4
&simrarr; => ⥲ e2a5b2
&slarr; => ← e28690
&smallsetminus; => ∖ e28896
&smashp; => ⨳ e2a8b3
&smeparsl; => ⧤ e2a7a4
&smid; => ∣ e288a3
&smile; => ⌣ e28ca3
&smt; => ⪪ e2aaaa
&smte; => ⪬ e2aaac
&smtes; => ⪬︀ e2aaacefb880
&softcy; => ь d18c
&sol; => / 2f
&solb; => ⧄ e2a784
&solbar; => ⌿ e28cbf
&sopf; => 𝕤 f09d95a4
&spades; => ♠ e299a0
&spadesuit; => ♠ e299a0
&spar; => ∥ e288a5
&sqcap; => ⊓ e28a93
&sqcaps; => ⊓︀ e28a93efb880
&sqcup; => ⊔ e28a94
&sqcups; => ⊔︀ e28a94efb880
&sqsub; => ⊏ e28a8f
&sqsube; => ⊑ e28a91
&sqsubset; => ⊏ e28a8f
&sqsubseteq; => ⊑ e28a91
&sqsup; => ⊐ e28a90
&sqsupe; => ⊒ e28a92
&sqsupset; => ⊐ e28a90
&sqsupseteq; => ⊒ e28a92
&squ; => □ e296a1
&square; => □ e296a1
&squarf; => ▪ e296aa
&squf; => ▪ e296aa
&srarr; => → e28692
&sscr; => 𝓈 f09d9388
&ssetmn; => ∖ e28896
&ssmile; => ⌣ e28ca3
&sstarf; => ⋆ e28b86
&star; => ☆ e29886
&starf; => ★ e29885
&straightepsilon; => ϵ cfb5
&straightphi; => ϕ cf95
&strns; => ¯ c2af
&sub; => ⊂ e28a82
&subE; => ⫅ e2ab85
&subdot; => ⪽ e2aabd
&sube; => ⊆ e28a86
&subedot; => ⫃ e2ab83
&submult; => ⫁ e2ab81
&subnE; => ⫋ e2ab8b
&subne; => ⊊ e28a8a
&subplus; => ⪿ e2aabf
&subrarr; => ⥹ e2a5b9
&subset; => ⊂ e28a82
&subseteq; => ⊆ e28a86
&subseteqq; => ⫅ e2ab85
&subsetneq; => ⊊ e28a8a
&subsetneqq; => ⫋ e2ab8b
&subsim; => ⫇ e2ab87
&subsub; => ⫕ e2ab95
&subsup; => ⫓ e2ab93
&succ; => ≻ e289bb
&succapprox; => ⪸ e2aab8
&succcurlyeq; => ≽ e289bd
&succeq; => ⪰ e2aab0
&succnapprox; => ⪺ e2aaba
&succneqq; => ⪶ e2aab6
&succnsim; => ⋩ e28ba9
&succsim; => ≿ e289bf
&sum; => ∑ e28891
&sung; => ♪ e299aa
&sup; => ⊃ e28a83
&sup1; => ¹ c2b9
&sup2; => ² c2b2
&sup3; => ³ c2b3
&supE; => ⫆ e2ab86
&supdot; => ⪾ e2aabe
&supdsub; => ⫘ e2ab98
&supe; => ⊇ e28a87
&supedot; => ⫄ e2ab84
&suphsol; => ⟉ e29f89
&suphsub; => ⫗ e2ab97
&suplarr; => ⥻ e2a5bb
&supmult; => ⫂ e2ab82
&supnE; => ⫌ e2ab8c
&supne; => ⊋ e28a8b
&supplus; => ⫀ e2ab80
&supset; => ⊃ e28a83
&supseteq; => ⊇ e28a87
&supseteqq; => ⫆ e2ab86
&supsetneq; => ⊋ e28a8b
&supsetneqq; => ⫌ e2ab8c
&supsim; => ⫈ e2ab88
&supsub; => ⫔ e2ab94
&supsup; => ⫖ e2ab96
&swArr; => ⇙ e28799
&swarhk; => ⤦ e2a4a6
&swarr; => ↙ e28699
&swarrow; => ↙ e28699
&swnwar; => ⤪ e2a4aa
&szlig; => ß c39f
&target; => ⌖ e28c96
&tau; => τ cf84
&tbrk; => ⎴ e28eb4
&tcaron; => ť c5a5
&tcedil; => ţ c5a3
&tcy; => т d182
&tdot; => ⃛ e2839b
&telrec; => ⌕ e28c95
&tfr; => 𝔱 f09d94b1
&there4; => ∴ e288b4
&therefore; => ∴ e288b4
&theta; => θ ceb8
&thetasym; => ϑ cf91
&thetav; => ϑ cf91
&thickapprox; => ≈ e28988
&thicksim; => ∼ e288bc
&thinsp; =>   e28089
&thkap; => ≈ e28988
&thksim; => ∼ e288bc
&thorn; => þ c3be
&tilde; => ˜ cb9c
&times; => × c397
&timesb; => ⊠ e28aa0
&timesbar; => ⨱ e2a8b1
&timesd; => ⨰ e2a8b0
&tint; => ∭ e288ad
&toea; => ⤨ e2a4a8
&top; => ⊤ e28aa4
&topbot; => ⌶ e28cb6
&topcir; => ⫱ e2abb1
&topf; => 𝕥 f09d95a5
&topfork; => ⫚ e2ab9a
&tosa; => ⤩ e2a4a9
&tprime; => ‴ e280b4
&trade; => ™ e284a2
&triangle; => ▵ e296b5
&triangledown; => ▿ e296bf
&triangleleft; => ◃ e29783
&trianglelefteq; => ⊴ e28ab4
&triangleq; => ≜ e2899c
&triangleright; => ▹ e296b9
&trianglerighteq; => ⊵ e28ab5
&tridot; => ◬ e297ac
&trie; => ≜ e2899c
&triminus; => ⨺ e2a8ba
&triplus; => ⨹ e2a8b9
&trisb; => ⧍ e2a78d
&tritime; => ⨻ e2a8bb
&trpezium; => ⏢ e28fa2
&tscr; => 𝓉 f09d9389
&tscy; => ц d186
&tshcy; => ћ d19b
&tstrok; => ŧ c5a7
&twixt; => ≬ e289ac
&twoheadleftarrow; => ↞ e2869e
&twoheadrightarrow; => ↠ e286a0
&uArr; => ⇑ e28791
&uHar; => ⥣ e2a5a3
&uacute; => ú c3ba
&uarr; => ↑ e28691
&ubrcy; => ў d19e
&ubreve; => ŭ c5ad
&ucirc; => û c3bb
&ucy; => у d183
&udarr; => ⇅ e28785
&udblac; => ű c5b1
&udhar; => ⥮ e2a5ae
&ufisht; => ⥾ e2a5be
&ufr; => 𝔲 f09d94b2
&ugrave; => ù c3b9
&uharl; => ↿ e286bf
&uharr; => ↾ e286be
&uhblk; => ▀ e29680
&ulcorn; => ⌜ e28c9c
&ulcorner; => ⌜ e28c9c
&ulcrop; => ⌏ e28c8f
&ultri; => ◸ e297b8
&umacr; => ū c5ab
&uml; => ¨ c2a8
&uogon; => ų c5b3
&uopf; => 𝕦 f09d95a6
&uparrow; => ↑ e28691
&updownarrow; => ↕ e28695
&upharpoonleft; => ↿ e286bf
&upharpoonright; => ↾ e286be
&uplus; => ⊎ e28a8e
&upsi; => υ cf85
&upsih; => ϒ cf92
&upsilon; => υ cf85
&upuparrows; => ⇈ e28788
&urcorn; => ⌝ e28c9d
&urcorner; => ⌝ e28c9d
&urcrop; => ⌎ e28c8e
&uring; => ů c5af
&urtri; => ◹ e297b9
&uscr; => 𝓊 f09d938a
&utdot; => ⋰ e28bb0
&utilde; => ũ c5a9
&utri; => ▵ e296b5
&utrif; => ▴ e296b4
&uuarr; => ⇈ e28788
&uuml; => ü c3bc
&uwangle; => ⦧ e2a6a7
&vArr; => ⇕ e28795
&vBar; => ⫨ e2aba8
&vBarv; => ⫩ e2aba9
&vDash; => ⊨ e28aa8
&vangrt; => ⦜ e2a69c
&varepsilon; => ϵ cfb5
&varkappa; => ϰ cfb0
&varnothing; => ∅ e28885
&varphi; => ϕ cf95
&varpi; => ϖ cf96
&varpropto; => ∝ e2889d
&varr; => ↕ e28695
&varrho; => ϱ cfb1
&varsigma; => ς cf82
&varsubsetneq; => ⊊︀ e28a8aefb880
&varsubsetneqq; => ⫋︀ e2ab8befb880
&varsupsetneq; => ⊋︀ e28a8befb880
&varsupsetneqq; => ⫌︀ e2ab8cefb880
&vartheta; => ϑ cf91
&vartriangleleft; => ⊲ e28ab2
&vartriangleright; => ⊳ e28ab3
&vcy; => в d0b2
&vdash; => ⊢ e28aa2
&vee; => ∨ e288a8
&veebar; => ⊻ e28abb
&veeeq; => ≚ e2899a
&vellip; => ⋮ e28bae
&verbar; => | 7c
&vert; => | 7c
&vfr; => 𝔳 f09d94b3
&vltri; => ⊲ e28ab2
&vnsub; => ⊂⃒ e28a82e28392
&vnsup; => ⊃⃒ e28a83e28392
&vopf; => 𝕧 f09d95a7
&vprop; => ∝ e2889d
&vrtri; => ⊳ e28ab3
&vscr; => 𝓋 f09d938b
&vsubnE; => ⫋︀ e2ab8befb880
&vsubne; => ⊊︀ e28a8aefb880
&vsupnE; => ⫌︀ e2ab8cefb880
&vsupne; => ⊋︀ e28a8befb880
&vzigzag; => ⦚ e2a69a
&wcirc; => ŵ c5b5
&wedbar; => ⩟ e2a99f
&wedge; => ∧ e288a7
&wedgeq; => ≙ e28999
&weierp; => ℘ e28498
&wfr; => 𝔴 f09d94b4
&wopf; => 𝕨 f09d95a8
&wp; => ℘ e28498
&wr; => ≀ e28980
&wreath; => ≀ e28980
&wscr; => 𝓌 f09d938c
&xcap; => ⋂ e28b82
&xcirc; => ◯ e297af
&xcup; => ⋃ e28b83
&xdtri; => ▽ e296bd
&xfr; => 𝔵 f09d94b5
&xhArr; => ⟺ e29fba
&xharr; => ⟷ e29fb7
&xi; => ξ cebe
&xlArr; => ⟸ e29fb8
&xlarr; => ⟵ e29fb5
&xmap; => ⟼ e29fbc
&xnis; => ⋻ e28bbb
&xodot; => ⨀ e2a880
&xopf; => 𝕩 f09d95a9
&xoplus; => ⨁ e2a881
&xotime; => ⨂ e2a882
&xrArr; => ⟹ e29fb9
&xrarr; => ⟶ e29fb6
&xscr; => 𝓍 f09d938d
&xsqcup; => ⨆ e2a886
&xuplus; => ⨄ e2a884
&xutri; => △ e296b3
&xvee; => ⋁ e28b81
&xwedge; => ⋀ e28b80
&yacute; => ý c3bd
&yacy; => я d18f
&ycirc; => ŷ c5b7
&ycy; => ы d18b
&yen; => ¥ c2a5
&yfr; => 𝔶 f09d94b6
&yicy; => ї d197
&yopf; => 𝕪 f09d95aa
&yscr; => 𝓎 f09d938e
&yucy; => ю d18e
&yuml; => ÿ c3bf
&zacute; => ź c5ba
&zcaron; => ž c5be
&zcy; => з d0b7
&zdot; => ż c5bc
&zeetrf; => ℨ e284a8
&zeta; => ζ ceb6
&zfr; => 𝔷 f09d94b7
&zhcy; => ж d0b6
&zigrarr; => ⇝ e2879d
&zopf; => 𝕫 f09d95ab
&zscr; => 𝓏 f09d938f
&zwj; => ‍ e2808d
&zwnj; => ‌ e2808c
