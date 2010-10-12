--TEST--
Test get_html_translation_table() function : basic functionality - with default args
--FILE--
<?php
/* Prototype  : array get_html_translation_table ( [int $table [, int $quote_style [, string charset_hint]]] )
 * Description: Returns the internal translation table used by htmlspecialchars and htmlentities
 * Source code: ext/standard/html.c
*/

/* Test get_html_translation_table() when table is specified as HTML_ENTITIES */


echo "*** Testing get_html_translation_table() : basic functionality ***\n";

echo "-- with table = HTML_ENTITIES --\n";
$table = HTML_ENTITIES;
var_dump( get_html_translation_table($table, ENT_COMPAT, "UTF-8") );

echo "-- with table = HTML_SPECIALCHARS --\n";
$table = HTML_SPECIALCHARS; 
var_dump( get_html_translation_table($table, ENT_COMPAT, "UTF-8") );

echo "Done\n";
?>
--EXPECTF--
*** Testing get_html_translation_table() : basic functionality ***
-- with table = HTML_ENTITIES --
array(252) {
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
  ["Œ"]=>
  string(7) "&OElig;"
  ["œ"]=>
  string(7) "&oelig;"
  ["Š"]=>
  string(8) "&Scaron;"
  ["š"]=>
  string(8) "&scaron;"
  ["Ÿ"]=>
  string(6) "&Yuml;"
  ["ƒ"]=>
  string(6) "&fnof;"
  ["ˆ"]=>
  string(6) "&circ;"
  ["˜"]=>
  string(7) "&tilde;"
  ["Α"]=>
  string(7) "&Alpha;"
  ["Β"]=>
  string(6) "&Beta;"
  ["Γ"]=>
  string(7) "&Gamma;"
  ["Δ"]=>
  string(7) "&Delta;"
  ["Ε"]=>
  string(9) "&Epsilon;"
  ["Ζ"]=>
  string(6) "&Zeta;"
  ["Η"]=>
  string(5) "&Eta;"
  ["Θ"]=>
  string(7) "&Theta;"
  ["Ι"]=>
  string(6) "&Iota;"
  ["Κ"]=>
  string(7) "&Kappa;"
  ["Λ"]=>
  string(8) "&Lambda;"
  ["Μ"]=>
  string(4) "&Mu;"
  ["Ν"]=>
  string(4) "&Nu;"
  ["Ξ"]=>
  string(4) "&Xi;"
  ["Ο"]=>
  string(9) "&Omicron;"
  ["Π"]=>
  string(4) "&Pi;"
  ["Ρ"]=>
  string(5) "&Rho;"
  ["Σ"]=>
  string(7) "&Sigma;"
  ["Τ"]=>
  string(5) "&Tau;"
  ["Υ"]=>
  string(9) "&Upsilon;"
  ["Φ"]=>
  string(5) "&Phi;"
  ["Χ"]=>
  string(5) "&Chi;"
  ["Ψ"]=>
  string(5) "&Psi;"
  ["Ω"]=>
  string(7) "&Omega;"
  ["α"]=>
  string(7) "&alpha;"
  ["β"]=>
  string(6) "&beta;"
  ["γ"]=>
  string(7) "&gamma;"
  ["δ"]=>
  string(7) "&delta;"
  ["ε"]=>
  string(9) "&epsilon;"
  ["ζ"]=>
  string(6) "&zeta;"
  ["η"]=>
  string(5) "&eta;"
  ["θ"]=>
  string(7) "&theta;"
  ["ι"]=>
  string(6) "&iota;"
  ["κ"]=>
  string(7) "&kappa;"
  ["λ"]=>
  string(8) "&lambda;"
  ["μ"]=>
  string(4) "&mu;"
  ["ν"]=>
  string(4) "&nu;"
  ["ξ"]=>
  string(4) "&xi;"
  ["ο"]=>
  string(9) "&omicron;"
  ["π"]=>
  string(4) "&pi;"
  ["ρ"]=>
  string(5) "&rho;"
  ["ς"]=>
  string(8) "&sigmaf;"
  ["σ"]=>
  string(7) "&sigma;"
  ["τ"]=>
  string(5) "&tau;"
  ["υ"]=>
  string(9) "&upsilon;"
  ["φ"]=>
  string(5) "&phi;"
  ["χ"]=>
  string(5) "&chi;"
  ["ψ"]=>
  string(5) "&psi;"
  ["ω"]=>
  string(7) "&omega;"
  ["ϑ"]=>
  string(10) "&thetasym;"
  ["ϒ"]=>
  string(7) "&upsih;"
  ["ϖ"]=>
  string(5) "&piv;"
  [" "]=>
  string(6) "&ensp;"
  [" "]=>
  string(6) "&emsp;"
  [" "]=>
  string(8) "&thinsp;"
  ["‌"]=>
  string(6) "&zwnj;"
  ["‍"]=>
  string(5) "&zwj;"
  ["‎"]=>
  string(5) "&lrm;"
  ["‏"]=>
  string(5) "&rlm;"
  ["–"]=>
  string(7) "&ndash;"
  ["—"]=>
  string(7) "&mdash;"
  ["‘"]=>
  string(7) "&lsquo;"
  ["’"]=>
  string(7) "&rsquo;"
  ["‚"]=>
  string(7) "&sbquo;"
  ["“"]=>
  string(7) "&ldquo;"
  ["”"]=>
  string(7) "&rdquo;"
  ["„"]=>
  string(7) "&bdquo;"
  ["†"]=>
  string(8) "&dagger;"
  ["‡"]=>
  string(8) "&Dagger;"
  ["•"]=>
  string(6) "&bull;"
  ["…"]=>
  string(8) "&hellip;"
  ["‰"]=>
  string(8) "&permil;"
  ["′"]=>
  string(7) "&prime;"
  ["″"]=>
  string(7) "&Prime;"
  ["‹"]=>
  string(8) "&lsaquo;"
  ["›"]=>
  string(8) "&rsaquo;"
  ["‾"]=>
  string(7) "&oline;"
  ["⁄"]=>
  string(7) "&frasl;"
  ["€"]=>
  string(6) "&euro;"
  ["ℑ"]=>
  string(7) "&image;"
  ["℘"]=>
  string(8) "&weierp;"
  ["ℜ"]=>
  string(6) "&real;"
  ["™"]=>
  string(7) "&trade;"
  ["ℵ"]=>
  string(9) "&alefsym;"
  ["←"]=>
  string(6) "&larr;"
  ["↑"]=>
  string(6) "&uarr;"
  ["→"]=>
  string(6) "&rarr;"
  ["↓"]=>
  string(6) "&darr;"
  ["↔"]=>
  string(6) "&harr;"
  ["↵"]=>
  string(7) "&crarr;"
  ["⇐"]=>
  string(6) "&lArr;"
  ["⇑"]=>
  string(6) "&uArr;"
  ["⇒"]=>
  string(6) "&rArr;"
  ["⇓"]=>
  string(6) "&dArr;"
  ["⇔"]=>
  string(6) "&hArr;"
  ["∀"]=>
  string(8) "&forall;"
  ["∂"]=>
  string(6) "&part;"
  ["∃"]=>
  string(7) "&exist;"
  ["∅"]=>
  string(7) "&empty;"
  ["∇"]=>
  string(7) "&nabla;"
  ["∈"]=>
  string(6) "&isin;"
  ["∉"]=>
  string(7) "&notin;"
  ["∋"]=>
  string(4) "&ni;"
  ["∏"]=>
  string(6) "&prod;"
  ["∑"]=>
  string(5) "&sum;"
  ["−"]=>
  string(7) "&minus;"
  ["∗"]=>
  string(8) "&lowast;"
  ["√"]=>
  string(7) "&radic;"
  ["∝"]=>
  string(6) "&prop;"
  ["∞"]=>
  string(7) "&infin;"
  ["∠"]=>
  string(5) "&ang;"
  ["∧"]=>
  string(5) "&and;"
  ["∨"]=>
  string(4) "&or;"
  ["∩"]=>
  string(5) "&cap;"
  ["∪"]=>
  string(5) "&cup;"
  ["∫"]=>
  string(5) "&int;"
  ["∴"]=>
  string(8) "&there4;"
  ["∼"]=>
  string(5) "&sim;"
  ["≅"]=>
  string(6) "&cong;"
  ["≈"]=>
  string(7) "&asymp;"
  ["≠"]=>
  string(4) "&ne;"
  ["≡"]=>
  string(7) "&equiv;"
  ["≤"]=>
  string(4) "&le;"
  ["≥"]=>
  string(4) "&ge;"
  ["⊂"]=>
  string(5) "&sub;"
  ["⊃"]=>
  string(5) "&sup;"
  ["⊄"]=>
  string(6) "&nsub;"
  ["⊆"]=>
  string(6) "&sube;"
  ["⊇"]=>
  string(6) "&supe;"
  ["⊕"]=>
  string(7) "&oplus;"
  ["⊗"]=>
  string(8) "&otimes;"
  ["⊥"]=>
  string(6) "&perp;"
  ["⋅"]=>
  string(6) "&sdot;"
  ["⌈"]=>
  string(7) "&lceil;"
  ["⌉"]=>
  string(7) "&rceil;"
  ["⌊"]=>
  string(8) "&lfloor;"
  ["⌋"]=>
  string(8) "&rfloor;"
  ["〈"]=>
  string(6) "&lang;"
  ["〉"]=>
  string(6) "&rang;"
  ["◊"]=>
  string(5) "&loz;"
  ["♠"]=>
  string(8) "&spades;"
  ["♣"]=>
  string(7) "&clubs;"
  ["♥"]=>
  string(8) "&hearts;"
  ["♦"]=>
  string(7) "&diams;"
  ["&"]=>
  string(5) "&amp;"
  ["""]=>
  string(6) "&quot;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
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
