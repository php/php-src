--TEST--
Test get_html_translation_table() function : basic functionality - with default args
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) == 'WIN'){  
  die('skip Not for Windows');
}

if( !setlocale(LC_ALL, "en_US.UTF-8") ) {
  die('skip failed to set locale settings to "en-US.UTF-8"');
}
?>
--FILE--
<?php
/* Prototype  : array get_html_translation_table ( [int $table [, int $quote_style]] )
 * Description: Returns the internal translation table used by htmlspecialchars and htmlentities
 * Source code: ext/standard/html.c
*/

/* Test get_html_translation_table() when table is specified as HTML_ENTITIES */

//set locale to en_US.UTF-8
setlocale(LC_ALL, "en_US.UTF-8"); 

echo "*** Testing get_html_translation_table() : basic functionality ***\n";

// Calling get_html_translation_table() with default arguments
echo "-- with default arguments --\n";
var_dump( get_html_translation_table() );

// Calling get_html_translation_table() with all possible optional arguments
echo "-- with table = HTML_ENTITIES --\n";
$table = HTML_ENTITIES;
var_dump( get_html_translation_table($table) );

echo "-- with table = HTML_SPECIALCHARS --\n";
$table = HTML_SPECIALCHARS; 
var_dump( get_html_translation_table($table) );

echo "Done\n";
?>
--EXPECTF--
*** Testing get_html_translation_table() : basic functionality ***
-- with default arguments --
array(4) {
  ["&"]=>
  string(5) "&amp;"
  ["""]=>
  string(6) "&quot;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
}
-- with table = HTML_ENTITIES --
array(171) {
  [" "]=>
  string(4) "&Pi;"
  ["¡"]=>
  string(5) "&Rho;"
  ["¢"]=>
  string(6) "&cent;"
  ["£"]=>
  string(7) "&Sigma;"
  ["¤"]=>
  string(5) "&Tau;"
  ["¥"]=>
  string(6) "&perp;"
  ["¦"]=>
  string(5) "&Phi;"
  ["§"]=>
  string(5) "&Chi;"
  ["¨"]=>
  string(5) "&Psi;"
  ["©"]=>
  string(7) "&Omega;"
  ["ª"]=>
  string(6) "&ordf;"
  ["«"]=>
  string(7) "&laquo;"
  ["¬"]=>
  string(6) "&euro;"
  ["­"]=>
  string(5) "&shy;"
  ["®"]=>
  string(5) "&reg;"
  ["¯"]=>
  string(6) "&macr;"
  ["°"]=>
  string(5) "&deg;"
  ["±"]=>
  string(7) "&alpha;"
  ["²"]=>
  string(6) "&beta;"
  ["³"]=>
  string(7) "&gamma;"
  ["´"]=>
  string(7) "&delta;"
  ["µ"]=>
  string(7) "&crarr;"
  ["¶"]=>
  string(6) "&zeta;"
  ["·"]=>
  string(5) "&eta;"
  ["¸"]=>
  string(7) "&theta;"
  ["¹"]=>
  string(6) "&iota;"
  ["º"]=>
  string(7) "&kappa;"
  ["»"]=>
  string(8) "&lambda;"
  ["¼"]=>
  string(4) "&mu;"
  ["½"]=>
  string(4) "&nu;"
  ["¾"]=>
  string(4) "&xi;"
  ["¿"]=>
  string(9) "&omicron;"
  ["À"]=>
  string(4) "&pi;"
  ["Á"]=>
  string(5) "&rho;"
  ["Â"]=>
  string(8) "&sigmaf;"
  ["Ã"]=>
  string(7) "&sigma;"
  ["Ä"]=>
  string(5) "&tau;"
  ["Å"]=>
  string(6) "&sdot;"
  ["Æ"]=>
  string(5) "&phi;"
  ["Ç"]=>
  string(5) "&chi;"
  ["È"]=>
  string(5) "&psi;"
  ["É"]=>
  string(7) "&omega;"
  ["Ê"]=>
  string(5) "&loz;"
  ["Ë"]=>
  string(6) "&Euml;"
  ["Ì"]=>
  string(8) "&Igrave;"
  ["Í"]=>
  string(8) "&Iacute;"
  ["Î"]=>
  string(7) "&Icirc;"
  ["Ï"]=>
  string(6) "&Iuml;"
  ["Ð"]=>
  string(6) "&lArr;"
  ["Ñ"]=>
  string(6) "&uArr;"
  ["Ò"]=>
  string(6) "&rArr;"
  ["Ó"]=>
  string(6) "&dArr;"
  ["Ô"]=>
  string(6) "&hArr;"
  ["Õ"]=>
  string(8) "&Otilde;"
  ["Ö"]=>
  string(5) "&piv;"
  ["×"]=>
  string(7) "&times;"
  ["Ø"]=>
  string(8) "&Oslash;"
  ["Ù"]=>
  string(8) "&Ugrave;"
  ["Ú"]=>
  string(8) "&Uacute;"
  ["Û"]=>
  string(7) "&Ucirc;"
  ["Ü"]=>
  string(7) "&tilde;"
  ["Ý"]=>
  string(8) "&Yacute;"
  ["Þ"]=>
  string(7) "&THORN;"
  ["ß"]=>
  string(7) "&szlig;"
  ["à"]=>
  string(8) "&agrave;"
  ["á"]=>
  string(8) "&aacute;"
  ["â"]=>
  string(7) "&acirc;"
  ["ã"]=>
  string(8) "&atilde;"
  ["ä"]=>
  string(6) "&auml;"
  ["å"]=>
  string(7) "&aring;"
  ["æ"]=>
  string(7) "&aelig;"
  ["ç"]=>
  string(8) "&ccedil;"
  ["è"]=>
  string(8) "&egrave;"
  ["é"]=>
  string(8) "&eacute;"
  ["ê"]=>
  string(7) "&ecirc;"
  ["ë"]=>
  string(6) "&euml;"
  ["ì"]=>
  string(8) "&igrave;"
  ["í"]=>
  string(8) "&iacute;"
  ["î"]=>
  string(7) "&icirc;"
  ["ï"]=>
  string(6) "&iuml;"
  ["ð"]=>
  string(5) "&eth;"
  ["ñ"]=>
  string(8) "&ntilde;"
  ["ò"]=>
  string(8) "&ograve;"
  ["ó"]=>
  string(8) "&oacute;"
  ["ô"]=>
  string(7) "&ocirc;"
  ["õ"]=>
  string(8) "&otilde;"
  ["ö"]=>
  string(6) "&ouml;"
  ["÷"]=>
  string(8) "&divide;"
  ["ø"]=>
  string(8) "&oslash;"
  ["ù"]=>
  string(8) "&ugrave;"
  ["ú"]=>
  string(8) "&uacute;"
  ["û"]=>
  string(7) "&ucirc;"
  ["ü"]=>
  string(6) "&uuml;"
  ["ý"]=>
  string(8) "&yacute;"
  ["þ"]=>
  string(7) "&thorn;"
  ["ÿ"]=>
  string(6) "&yuml;"
  ["R"]=>
  string(7) "&OElig;"
  ["S"]=>
  string(7) "&oelig;"
  ["`"]=>
  string(8) "&spades;"
  ["a"]=>
  string(7) "&equiv;"
  ["x"]=>
  string(6) "&Yuml;"
  ["’"]=>
  string(6) "&rarr;"
  ["‘"]=>
  string(6) "&uarr;"
  ["“"]=>
  string(6) "&darr;"
  ["”"]=>
  string(6) "&harr;"
  ["•"]=>
  string(7) "&oplus;"
  ["–"]=>
  string(6) "&Zeta;"
  ["—"]=>
  string(8) "&otimes;"
  ["˜"]=>
  string(7) "&Theta;"
  ["™"]=>
  string(6) "&Iota;"
  ["š"]=>
  string(7) "&Kappa;"
  ["›"]=>
  string(8) "&Lambda;"
  ["œ"]=>
  string(4) "&Mu;"
  [""]=>
  string(4) "&Nu;"
  ["ž"]=>
  string(4) "&Xi;"
  ["Ÿ"]=>
  string(9) "&Omicron;"
  [""]=>
  string(6) "&part;"
  [""]=>
  string(7) "&exist;"
  ["	"]=>
  string(7) "&rceil;"
  [""]=>
  string(6) "&zwnj;"
  [""]=>
  string(5) "&zwj;"
  [""]=>
  string(5) "&lrm;"
  [""]=>
  string(6) "&prod;"
  [""]=>
  string(7) "&ndash;"
  [""]=>
  string(7) "&mdash;"
  [""]=>
  string(8) "&weierp;"
  [""]=>
  string(7) "&rsquo;"
  [""]=>
  string(7) "&radic;"
  [""]=>
  string(6) "&real;"
  [""]=>
  string(6) "&prop;"
  [""]=>
  string(7) "&infin;"
  [" "]=>
  string(5) "&ang;"
  ["!"]=>
  string(8) "&Dagger;"
  ["""]=>
  string(6) "&quot;"
  ["&"]=>
  string(5) "&amp;"
  [0]=>
  string(8) "&permil;"
  [2]=>
  string(7) "&prime;"
  [3]=>
  string(7) "&Prime;"
  [9]=>
  string(8) "&lsaquo;"
  [":"]=>
  string(8) "&rsaquo;"
  [">"]=>
  string(4) "&gt;"
  ["D"]=>
  string(7) "&frasl;"
  [""]=>
  string(5) "&sum;"
  [5]=>
  string(9) "&alefsym;"
  [""]=>
  string(6) "&larr;"
  [""]=>
  string(8) "&forall;"
  [""]=>
  string(7) "&empty;"
  [""]=>
  string(7) "&nabla;"
  [""]=>
  string(7) "&lceil;"
  [""]=>
  string(8) "&rfloor;"
  [""]=>
  string(7) "&minus;"
  [""]=>
  string(8) "&lowast;"
  ["'"]=>
  string(5) "&and;"
  ["("]=>
  string(4) "&or;"
  [")"]=>
  string(6) "&lang;"
  ["*"]=>
  string(6) "&rang;"
  ["+"]=>
  string(5) "&int;"
  [4]=>
  string(8) "&there4;"
  ["<"]=>
  string(4) "&lt;"
  ["E"]=>
  string(6) "&cong;"
  ["H"]=>
  string(7) "&asymp;"
  ["d"]=>
  string(4) "&le;"
  ["e"]=>
  string(8) "&hearts;"
  ["‚"]=>
  string(5) "&sub;"
  ["ƒ"]=>
  string(5) "&sup;"
  ["„"]=>
  string(6) "&nsub;"
  ["†"]=>
  string(6) "&sube;"
  ["‡"]=>
  string(6) "&supe;"
  ["
"]=>
  string(8) "&lfloor;"
  ["c"]=>
  string(7) "&clubs;"
  ["f"]=>
  string(7) "&diams;"
}
-- with table = HTML_SPECIALCHARS --
array(4) {
  ["&"]=>
  string(5) "&amp;"
  ["""]=>
  string(6) "&quot;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
}
Done
