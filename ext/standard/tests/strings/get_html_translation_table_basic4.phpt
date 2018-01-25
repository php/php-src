--TEST--
Test get_html_translation_table() function : basic functionality - charset WINDOWS-1252
--FILE--
<?php
/* Prototype  : array get_html_translation_table ( [int $table [, int $quote_style [, string charset_hint]]] )
 * Description: Returns the internal translation table used by htmlspecialchars and htmlentities
 * Source code: ext/standard/html.c
*/


echo "*** Testing get_html_translation_table() : basic functionality/Windows-1252 ***\n";

echo "-- with table = HTML_ENTITIES --\n";
$table = HTML_ENTITIES;
$tt = get_html_translation_table($table, ENT_COMPAT, "WINDOWS-1252");
asort( $tt );
var_dump( $tt );

echo "-- with table = HTML_SPECIALCHARS --\n";
$table = HTML_SPECIALCHARS;
$tt = get_html_translation_table($table, ENT_COMPAT, "WINDOWS-1252");
asort( $tt );
var_dump( $tt );

echo "Done\n";
?>
--EXPECTF--
*** Testing get_html_translation_table() : basic functionality/Windows-1252 ***
-- with table = HTML_ENTITIES --
array(125) {
  ["Æ"]=>
  string(7) "&AElig;"
  ["Á"]=>
  string(8) "&Aacute;"
  ["Â"]=>
  string(7) "&Acirc;"
  ["À"]=>
  string(8) "&Agrave;"
  ["Å"]=>
  string(7) "&Aring;"
  ["Ã"]=>
  string(8) "&Atilde;"
  ["Ä"]=>
  string(6) "&Auml;"
  ["Ç"]=>
  string(8) "&Ccedil;"
  ["‡"]=>
  string(8) "&Dagger;"
  ["Ð"]=>
  string(5) "&ETH;"
  ["É"]=>
  string(8) "&Eacute;"
  ["Ê"]=>
  string(7) "&Ecirc;"
  ["È"]=>
  string(8) "&Egrave;"
  ["Ë"]=>
  string(6) "&Euml;"
  ["Í"]=>
  string(8) "&Iacute;"
  ["Î"]=>
  string(7) "&Icirc;"
  ["Ì"]=>
  string(8) "&Igrave;"
  ["Ï"]=>
  string(6) "&Iuml;"
  ["Ñ"]=>
  string(8) "&Ntilde;"
  ["Œ"]=>
  string(7) "&OElig;"
  ["Ó"]=>
  string(8) "&Oacute;"
  ["Ô"]=>
  string(7) "&Ocirc;"
  ["Ò"]=>
  string(8) "&Ograve;"
  ["Ø"]=>
  string(8) "&Oslash;"
  ["Õ"]=>
  string(8) "&Otilde;"
  ["Ö"]=>
  string(6) "&Ouml;"
  ["Š"]=>
  string(8) "&Scaron;"
  ["Þ"]=>
  string(7) "&THORN;"
  ["Ú"]=>
  string(8) "&Uacute;"
  ["Û"]=>
  string(7) "&Ucirc;"
  ["Ù"]=>
  string(8) "&Ugrave;"
  ["Ü"]=>
  string(6) "&Uuml;"
  ["Ý"]=>
  string(8) "&Yacute;"
  ["Ÿ"]=>
  string(6) "&Yuml;"
  ["á"]=>
  string(8) "&aacute;"
  ["â"]=>
  string(7) "&acirc;"
  ["´"]=>
  string(7) "&acute;"
  ["æ"]=>
  string(7) "&aelig;"
  ["à"]=>
  string(8) "&agrave;"
  ["&"]=>
  string(5) "&amp;"
  ["å"]=>
  string(7) "&aring;"
  ["ã"]=>
  string(8) "&atilde;"
  ["ä"]=>
  string(6) "&auml;"
  ["„"]=>
  string(7) "&bdquo;"
  ["¦"]=>
  string(8) "&brvbar;"
  ["•"]=>
  string(6) "&bull;"
  ["ç"]=>
  string(8) "&ccedil;"
  ["¸"]=>
  string(7) "&cedil;"
  ["¢"]=>
  string(6) "&cent;"
  ["ˆ"]=>
  string(6) "&circ;"
  ["©"]=>
  string(6) "&copy;"
  ["¤"]=>
  string(8) "&curren;"
  ["†"]=>
  string(8) "&dagger;"
  ["°"]=>
  string(5) "&deg;"
  ["÷"]=>
  string(8) "&divide;"
  ["é"]=>
  string(8) "&eacute;"
  ["ê"]=>
  string(7) "&ecirc;"
  ["è"]=>
  string(8) "&egrave;"
  ["ð"]=>
  string(5) "&eth;"
  ["ë"]=>
  string(6) "&euml;"
  ["€"]=>
  string(6) "&euro;"
  ["ƒ"]=>
  string(6) "&fnof;"
  ["½"]=>
  string(8) "&frac12;"
  ["¼"]=>
  string(8) "&frac14;"
  ["¾"]=>
  string(8) "&frac34;"
  [">"]=>
  string(4) "&gt;"
  ["…"]=>
  string(8) "&hellip;"
  ["í"]=>
  string(8) "&iacute;"
  ["î"]=>
  string(7) "&icirc;"
  ["¡"]=>
  string(7) "&iexcl;"
  ["ì"]=>
  string(8) "&igrave;"
  ["¿"]=>
  string(8) "&iquest;"
  ["ï"]=>
  string(6) "&iuml;"
  ["«"]=>
  string(7) "&laquo;"
  ["“"]=>
  string(7) "&ldquo;"
  ["‹"]=>
  string(8) "&lsaquo;"
  ["‘"]=>
  string(7) "&lsquo;"
  ["<"]=>
  string(4) "&lt;"
  ["¯"]=>
  string(6) "&macr;"
  ["—"]=>
  string(7) "&mdash;"
  ["µ"]=>
  string(7) "&micro;"
  ["·"]=>
  string(8) "&middot;"
  [" "]=>
  string(6) "&nbsp;"
  ["–"]=>
  string(7) "&ndash;"
  ["¬"]=>
  string(5) "&not;"
  ["ñ"]=>
  string(8) "&ntilde;"
  ["ó"]=>
  string(8) "&oacute;"
  ["ô"]=>
  string(7) "&ocirc;"
  ["œ"]=>
  string(7) "&oelig;"
  ["ò"]=>
  string(8) "&ograve;"
  ["ª"]=>
  string(6) "&ordf;"
  ["º"]=>
  string(6) "&ordm;"
  ["ø"]=>
  string(8) "&oslash;"
  ["õ"]=>
  string(8) "&otilde;"
  ["ö"]=>
  string(6) "&ouml;"
  ["¶"]=>
  string(6) "&para;"
  ["‰"]=>
  string(8) "&permil;"
  ["±"]=>
  string(8) "&plusmn;"
  ["£"]=>
  string(7) "&pound;"
  ["""]=>
  string(6) "&quot;"
  ["»"]=>
  string(7) "&raquo;"
  ["”"]=>
  string(7) "&rdquo;"
  ["®"]=>
  string(5) "&reg;"
  ["›"]=>
  string(8) "&rsaquo;"
  ["’"]=>
  string(7) "&rsquo;"
  ["‚"]=>
  string(7) "&sbquo;"
  ["š"]=>
  string(8) "&scaron;"
  ["§"]=>
  string(6) "&sect;"
  ["­"]=>
  string(5) "&shy;"
  ["¹"]=>
  string(6) "&sup1;"
  ["²"]=>
  string(6) "&sup2;"
  ["³"]=>
  string(6) "&sup3;"
  ["ß"]=>
  string(7) "&szlig;"
  ["þ"]=>
  string(7) "&thorn;"
  ["˜"]=>
  string(7) "&tilde;"
  ["×"]=>
  string(7) "&times;"
  ["™"]=>
  string(7) "&trade;"
  ["ú"]=>
  string(8) "&uacute;"
  ["û"]=>
  string(7) "&ucirc;"
  ["ù"]=>
  string(8) "&ugrave;"
  ["¨"]=>
  string(5) "&uml;"
  ["ü"]=>
  string(6) "&uuml;"
  ["ý"]=>
  string(8) "&yacute;"
  ["¥"]=>
  string(5) "&yen;"
  ["ÿ"]=>
  string(6) "&yuml;"
}
-- with table = HTML_SPECIALCHARS --
array(4) {
  ["&"]=>
  string(5) "&amp;"
  [">"]=>
  string(4) "&gt;"
  ["<"]=>
  string(4) "&lt;"
  ["""]=>
  string(6) "&quot;"
}
Done
