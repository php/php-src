--TEST--
Test get_html_translation_table() function : basic functionality - table as HTML_ENTITIES & diff quote_style
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) != "WIN"){  
  die('skip only for Windows');
}

if( !setlocale(LC_ALL, "English_United States.1252") ) {
  die('skip failed to set locale settings to "English_United States.1252"');
}

?>
--FILE--
<?php
/* Prototype  : array get_html_translation_table ( [int $table [, int $quote_style]] )
 * Description: Returns the internal translation table used by htmlspecialchars and htmlentities
 * Source code: ext/standard/html.c
*/

/* Test get_html_translation_table() when table is specified as HTML_ENTITIES */

//set locale 
setlocale(LC_ALL, "English_United States.1252");


echo "*** Testing get_html_translation_table() : basic functionality ***\n";

// Calling get_html_translation_table() with default arguments
echo "-- with default arguments --\n";
var_dump( get_html_translation_table() );

// Calling get_html_translation_table() with all arguments
// $table as HTML_ENTITIES and different quote style
echo "-- with table = HTML_ENTITIES & quote_style = ENT_COMPAT --\n";
$table = HTML_ENTITIES;
$quote_style = ENT_COMPAT;
var_dump( get_html_translation_table($table, $quote_style) );

echo "-- with table = HTML_ENTITIES & quote_style = ENT_QUOTES --\n";
$quote_style = ENT_QUOTES;
var_dump( get_html_translation_table($table, $quote_style) );

echo "-- with table = HTML_ENTITIES & quote_style = ENT_NOQUOTES --\n";
$quote_style = ENT_NOQUOTES;
var_dump( get_html_translation_table($table, $quote_style) );


echo "Done\n";
?>
--EXPECTF--
*** Testing get_html_translation_table() : basic functionality ***
-- with default arguments --
array(4) {
  ["""]=>
  string(6) "&quot;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
-- with table = HTML_ENTITIES & quote_style = ENT_COMPAT --
array(100) {
  [" "]=>
  string(6) "&nbsp;"
  ["¡"]=>
  string(7) "&iexcl;"
  ["¢"]=>
  string(6) "&cent;"
  ["£"]=>
  string(7) "&pound;"
  ["¤"]=>
  string(8) "&curren;"
  ["¥"]=>
  string(5) "&yen;"
  ["¦"]=>
  string(8) "&brvbar;"
  ["§"]=>
  string(6) "&sect;"
  ["¨"]=>
  string(5) "&uml;"
  ["©"]=>
  string(6) "&copy;"
  ["ª"]=>
  string(6) "&ordf;"
  ["«"]=>
  string(7) "&laquo;"
  ["¬"]=>
  string(5) "&not;"
  ["­"]=>
  string(5) "&shy;"
  ["®"]=>
  string(5) "&reg;"
  ["¯"]=>
  string(6) "&macr;"
  ["°"]=>
  string(5) "&deg;"
  ["±"]=>
  string(8) "&plusmn;"
  ["²"]=>
  string(6) "&sup2;"
  ["³"]=>
  string(6) "&sup3;"
  ["´"]=>
  string(7) "&acute;"
  ["µ"]=>
  string(7) "&micro;"
  ["¶"]=>
  string(6) "&para;"
  ["·"]=>
  string(8) "&middot;"
  ["¸"]=>
  string(7) "&cedil;"
  ["¹"]=>
  string(6) "&sup1;"
  ["º"]=>
  string(6) "&ordm;"
  ["»"]=>
  string(7) "&raquo;"
  ["¼"]=>
  string(8) "&frac14;"
  ["½"]=>
  string(8) "&frac12;"
  ["¾"]=>
  string(8) "&frac34;"
  ["¿"]=>
  string(8) "&iquest;"
  ["À"]=>
  string(8) "&Agrave;"
  ["Á"]=>
  string(8) "&Aacute;"
  ["Â"]=>
  string(7) "&Acirc;"
  ["Ã"]=>
  string(8) "&Atilde;"
  ["Ä"]=>
  string(6) "&Auml;"
  ["Å"]=>
  string(7) "&Aring;"
  ["Æ"]=>
  string(7) "&AElig;"
  ["Ç"]=>
  string(8) "&Ccedil;"
  ["È"]=>
  string(8) "&Egrave;"
  ["É"]=>
  string(8) "&Eacute;"
  ["Ê"]=>
  string(7) "&Ecirc;"
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
  string(5) "&ETH;"
  ["Ñ"]=>
  string(8) "&Ntilde;"
  ["Ò"]=>
  string(8) "&Ograve;"
  ["Ó"]=>
  string(8) "&Oacute;"
  ["Ô"]=>
  string(7) "&Ocirc;"
  ["Õ"]=>
  string(8) "&Otilde;"
  ["Ö"]=>
  string(6) "&Ouml;"
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
  string(6) "&Uuml;"
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
  ["""]=>
  string(6) "&quot;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
-- with table = HTML_ENTITIES & quote_style = ENT_QUOTES --
array(101) {
  [" "]=>
  string(6) "&nbsp;"
  ["¡"]=>
  string(7) "&iexcl;"
  ["¢"]=>
  string(6) "&cent;"
  ["£"]=>
  string(7) "&pound;"
  ["¤"]=>
  string(8) "&curren;"
  ["¥"]=>
  string(5) "&yen;"
  ["¦"]=>
  string(8) "&brvbar;"
  ["§"]=>
  string(6) "&sect;"
  ["¨"]=>
  string(5) "&uml;"
  ["©"]=>
  string(6) "&copy;"
  ["ª"]=>
  string(6) "&ordf;"
  ["«"]=>
  string(7) "&laquo;"
  ["¬"]=>
  string(5) "&not;"
  ["­"]=>
  string(5) "&shy;"
  ["®"]=>
  string(5) "&reg;"
  ["¯"]=>
  string(6) "&macr;"
  ["°"]=>
  string(5) "&deg;"
  ["±"]=>
  string(8) "&plusmn;"
  ["²"]=>
  string(6) "&sup2;"
  ["³"]=>
  string(6) "&sup3;"
  ["´"]=>
  string(7) "&acute;"
  ["µ"]=>
  string(7) "&micro;"
  ["¶"]=>
  string(6) "&para;"
  ["·"]=>
  string(8) "&middot;"
  ["¸"]=>
  string(7) "&cedil;"
  ["¹"]=>
  string(6) "&sup1;"
  ["º"]=>
  string(6) "&ordm;"
  ["»"]=>
  string(7) "&raquo;"
  ["¼"]=>
  string(8) "&frac14;"
  ["½"]=>
  string(8) "&frac12;"
  ["¾"]=>
  string(8) "&frac34;"
  ["¿"]=>
  string(8) "&iquest;"
  ["À"]=>
  string(8) "&Agrave;"
  ["Á"]=>
  string(8) "&Aacute;"
  ["Â"]=>
  string(7) "&Acirc;"
  ["Ã"]=>
  string(8) "&Atilde;"
  ["Ä"]=>
  string(6) "&Auml;"
  ["Å"]=>
  string(7) "&Aring;"
  ["Æ"]=>
  string(7) "&AElig;"
  ["Ç"]=>
  string(8) "&Ccedil;"
  ["È"]=>
  string(8) "&Egrave;"
  ["É"]=>
  string(8) "&Eacute;"
  ["Ê"]=>
  string(7) "&Ecirc;"
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
  string(5) "&ETH;"
  ["Ñ"]=>
  string(8) "&Ntilde;"
  ["Ò"]=>
  string(8) "&Ograve;"
  ["Ó"]=>
  string(8) "&Oacute;"
  ["Ô"]=>
  string(7) "&Ocirc;"
  ["Õ"]=>
  string(8) "&Otilde;"
  ["Ö"]=>
  string(6) "&Ouml;"
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
  string(6) "&Uuml;"
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
  ["""]=>
  string(6) "&quot;"
  ["'"]=>
  string(5) "&#39;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
-- with table = HTML_ENTITIES & quote_style = ENT_NOQUOTES --
array(99) {
  [" "]=>
  string(6) "&nbsp;"
  ["¡"]=>
  string(7) "&iexcl;"
  ["¢"]=>
  string(6) "&cent;"
  ["£"]=>
  string(7) "&pound;"
  ["¤"]=>
  string(8) "&curren;"
  ["¥"]=>
  string(5) "&yen;"
  ["¦"]=>
  string(8) "&brvbar;"
  ["§"]=>
  string(6) "&sect;"
  ["¨"]=>
  string(5) "&uml;"
  ["©"]=>
  string(6) "&copy;"
  ["ª"]=>
  string(6) "&ordf;"
  ["«"]=>
  string(7) "&laquo;"
  ["¬"]=>
  string(5) "&not;"
  ["­"]=>
  string(5) "&shy;"
  ["®"]=>
  string(5) "&reg;"
  ["¯"]=>
  string(6) "&macr;"
  ["°"]=>
  string(5) "&deg;"
  ["±"]=>
  string(8) "&plusmn;"
  ["²"]=>
  string(6) "&sup2;"
  ["³"]=>
  string(6) "&sup3;"
  ["´"]=>
  string(7) "&acute;"
  ["µ"]=>
  string(7) "&micro;"
  ["¶"]=>
  string(6) "&para;"
  ["·"]=>
  string(8) "&middot;"
  ["¸"]=>
  string(7) "&cedil;"
  ["¹"]=>
  string(6) "&sup1;"
  ["º"]=>
  string(6) "&ordm;"
  ["»"]=>
  string(7) "&raquo;"
  ["¼"]=>
  string(8) "&frac14;"
  ["½"]=>
  string(8) "&frac12;"
  ["¾"]=>
  string(8) "&frac34;"
  ["¿"]=>
  string(8) "&iquest;"
  ["À"]=>
  string(8) "&Agrave;"
  ["Á"]=>
  string(8) "&Aacute;"
  ["Â"]=>
  string(7) "&Acirc;"
  ["Ã"]=>
  string(8) "&Atilde;"
  ["Ä"]=>
  string(6) "&Auml;"
  ["Å"]=>
  string(7) "&Aring;"
  ["Æ"]=>
  string(7) "&AElig;"
  ["Ç"]=>
  string(8) "&Ccedil;"
  ["È"]=>
  string(8) "&Egrave;"
  ["É"]=>
  string(8) "&Eacute;"
  ["Ê"]=>
  string(7) "&Ecirc;"
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
  string(5) "&ETH;"
  ["Ñ"]=>
  string(8) "&Ntilde;"
  ["Ò"]=>
  string(8) "&Ograve;"
  ["Ó"]=>
  string(8) "&Oacute;"
  ["Ô"]=>
  string(7) "&Ocirc;"
  ["Õ"]=>
  string(8) "&Otilde;"
  ["Ö"]=>
  string(6) "&Ouml;"
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
  string(6) "&Uuml;"
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
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
Done
