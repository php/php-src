--TEST--
FriBidi charsets
--SKIPIF--
<?php if (!extension_loaded("fribidi")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
	error_reporting(E_ALL ^ E_NOTICE);

	echo FRIBIDI_CHARSET_8859_8."\n";
	echo FRIBIDI_CHARSET_8859_6."\n";
	echo FRIBIDI_CHARSET_UTF8."\n";
	echo FRIBIDI_CHARSET_CP1255."\n";
	echo FRIBIDI_CHARSET_CP1256."\n";
	echo FRIBIDI_CHARSET_ISIRI_3342."\n";
	echo FRIBIDI_CHARSET_CAP_RTL."\n";
	
	$charsets = fribidi_get_charsets();
	foreach ($charsets as $k => $charset) {
		print_r(fribidi_charset_info($k));
	}

?>
--EXPECT--
4
3
1
5
6
7
2
Array
(
    [name] => UTF-8
    [title] => UTF-8 (Unicode)
)
Array
(
    [name] => CapRTL
    [title] => CapRTL (Test)
    [desc] => CapRTL is a character set for testing with the reference
implementation, with explicit marks escape strings, and
the property that contains all unicode character types in
ASCII range 1-127.

Warning: CapRTL character types are subject to change.

CapRTL's character types:

  * 0x00 ^@ ON    * 0x01 ^A ON    * 0x02 ^B ON    * 0x03 ^C ON  
  * 0x04 ^D LTR   * 0x05 ^E RTL   * 0x06 ^F ON    * 0x07 ^G ON  
  * 0x08 ^H ON    * 0x09 ^I ON    * 0x0a ^J ON    * 0x0b ^K ON  
  * 0x0c ^L ON    * 0x0d ^M BS    * 0x0e ^N RLO   * 0x0f ^O RLE 
  * 0x10 ^P LRO   * 0x11 ^Q LRE   * 0x12 ^R PDF   * 0x13 ^S WS  
  * 0x14 ^T ON    * 0x15 ^U ON    * 0x16 ^V ON    * 0x17 ^W ON  
  * 0x18 ^X ON    * 0x19 ^Y ON    * 0x1a ^Z ON    * 0x1b ^[ ON  
  * 0x1c ^\ ON    * 0x1d ^] ON    * 0x1e ^^ ON    * 0x1f ^_ ON  
  * 0x20    WS    * 0x21  ! ON    * 0x22  " ON    * 0x23  # ON  
  * 0x24  $ ET    * 0x25  % ON    * 0x26  & ON    * 0x27  ' ON  
  * 0x28  ( ON    * 0x29  ) ON    * 0x2a  * ON    * 0x2b  + ET  
  * 0x2c  , CS    * 0x2d  - ON    * 0x2e  . ES    * 0x2f  / ES  
  * 0x30  0 EN    * 0x31  1 EN    * 0x32  2 EN    * 0x33  3 EN  
  * 0x34  4 EN    * 0x35  5 EN    * 0x36  6 AN    * 0x37  7 AN  
  * 0x38  8 AN    * 0x39  9 AN    * 0x3a  : CS    * 0x3b  ; ON  
  * 0x3c  < ON    * 0x3d  = ON    * 0x3e  > ON    * 0x3f  ? ON  
  * 0x40  @ RTL   * 0x41  A AL    * 0x42  B AL    * 0x43  C AL  
  * 0x44  D AL    * 0x45  E AL    * 0x46  F AL    * 0x47  G RTL 
  * 0x48  H RTL   * 0x49  I RTL   * 0x4a  J RTL   * 0x4b  K RTL 
  * 0x4c  L RTL   * 0x4d  M RTL   * 0x4e  N RTL   * 0x4f  O RTL 
  * 0x50  P RTL   * 0x51  Q RTL   * 0x52  R RTL   * 0x53  S RTL 
  * 0x54  T RTL   * 0x55  U RTL   * 0x56  V RTL   * 0x57  W RTL 
  * 0x58  X RTL   * 0x59  Y RTL   * 0x5a  Z RTL   * 0x5b  [ ON  
  * 0x5c  \ BS    * 0x5d  ] ON    * 0x5e  ^ ON    * 0x5f  _ ON  
  * 0x60  ` NSM   * 0x61  a LTR   * 0x62  b LTR   * 0x63  c LTR 
  * 0x64  d LTR   * 0x65  e LTR   * 0x66  f LTR   * 0x67  g LTR 
  * 0x68  h LTR   * 0x69  i LTR   * 0x6a  j LTR   * 0x6b  k LTR 
  * 0x6c  l LTR   * 0x6d  m LTR   * 0x6e  n LTR   * 0x6f  o LTR 
  * 0x70  p LTR   * 0x71  q LTR   * 0x72  r LTR   * 0x73  s LTR 
  * 0x74  t LTR   * 0x75  u LTR   * 0x76  v LTR   * 0x77  w LTR 
  * 0x78  x LTR   * 0x79  y LTR   * 0x7a  z LTR   * 0x7b  { ON  
  * 0x7c  | SS    * 0x7d  } ON    * 0x7e  ~ WS    * 0x7f    ON  

Escape sequences:
  Character `_' is used to escape explicit marks. The list is:
    * _>  LRM
    * _<  RLM
    * _l  LRE
    * _r  RLE
    * _L  LRO
    * _R  RLO
    * _o  PDF
    * __  `_' itself


)
Array
(
    [name] => ISO8859-6
    [title] => ISO 8859-6 (Arabic)
)
Array
(
    [name] => ISO8859-8
    [title] => ISO 8859-8 (Hebrew)
)
Array
(
    [name] => CP1255
    [title] => CP1255 (Hebrew/Yiddish)
)
Array
(
    [name] => CP1256
    [title] => CP1256 (MS-Arabic)
)
Array
(
    [name] => ISIRI-3342
    [title] => ISIRI 3342 (Persian)
)
