--TEST--
Test get_html_translation_table() function : basic functionality - table as HTML_ENTITIES & diff quote_style
--FILE--
<?php
/* Test get_html_translation_table() when table is specified as HTML_ENTITIES */

//set locale to en_US.UTF-8
setlocale(LC_ALL, "en_US.UTF-8");


echo "*** Testing get_html_translation_table() : basic functionality ***\n";

// Calling get_html_translation_table() with all arguments
// $table as HTML_ENTITIES and different quote style
echo "-- with table = HTML_ENTITIES & quote_style = ENT_COMPAT --\n";
$table = HTML_ENTITIES;
$quote_style = ENT_COMPAT;
$tt = get_html_translation_table($table, $quote_style, "UTF-8");
asort( $tt );
var_dump( $tt );

echo "-- with table = HTML_ENTITIES & quote_style = ENT_QUOTES --\n";
$quote_style = ENT_QUOTES;
$tt = get_html_translation_table($table, $quote_style, "UTF-8");
asort( $tt );
var_dump( $tt );

echo "-- with table = HTML_ENTITIES & quote_style = ENT_NOQUOTES --\n";
$quote_style = ENT_NOQUOTES;
$tt = get_html_translation_table($table, $quote_style, "UTF-8");
asort( $tt );
var_dump( $tt );


echo "Done\n";
?>
--EXPECT--
*** Testing get_html_translation_table() : basic functionality ***
-- with table = HTML_ENTITIES & quote_style = ENT_COMPAT --
array(252) {
  ["Æ"]=>
  string(7) "&AElig;"
  ["Á"]=>
  string(8) "&Aacute;"
  ["Â"]=>
  string(7) "&Acirc;"
  ["À"]=>
  string(8) "&Agrave;"
  ["Α"]=>
  string(7) "&Alpha;"
  ["Å"]=>
  string(7) "&Aring;"
  ["Ã"]=>
  string(8) "&Atilde;"
  ["Ä"]=>
  string(6) "&Auml;"
  ["Β"]=>
  string(6) "&Beta;"
  ["Ç"]=>
  string(8) "&Ccedil;"
  ["Χ"]=>
  string(5) "&Chi;"
  ["‡"]=>
  string(8) "&Dagger;"
  ["Δ"]=>
  string(7) "&Delta;"
  ["Ð"]=>
  string(5) "&ETH;"
  ["É"]=>
  string(8) "&Eacute;"
  ["Ê"]=>
  string(7) "&Ecirc;"
  ["È"]=>
  string(8) "&Egrave;"
  ["Ε"]=>
  string(9) "&Epsilon;"
  ["Η"]=>
  string(5) "&Eta;"
  ["Ë"]=>
  string(6) "&Euml;"
  ["Γ"]=>
  string(7) "&Gamma;"
  ["Í"]=>
  string(8) "&Iacute;"
  ["Î"]=>
  string(7) "&Icirc;"
  ["Ì"]=>
  string(8) "&Igrave;"
  ["Ι"]=>
  string(6) "&Iota;"
  ["Ï"]=>
  string(6) "&Iuml;"
  ["Κ"]=>
  string(7) "&Kappa;"
  ["Λ"]=>
  string(8) "&Lambda;"
  ["Μ"]=>
  string(4) "&Mu;"
  ["Ñ"]=>
  string(8) "&Ntilde;"
  ["Ν"]=>
  string(4) "&Nu;"
  ["Œ"]=>
  string(7) "&OElig;"
  ["Ó"]=>
  string(8) "&Oacute;"
  ["Ô"]=>
  string(7) "&Ocirc;"
  ["Ò"]=>
  string(8) "&Ograve;"
  ["Ω"]=>
  string(7) "&Omega;"
  ["Ο"]=>
  string(9) "&Omicron;"
  ["Ø"]=>
  string(8) "&Oslash;"
  ["Õ"]=>
  string(8) "&Otilde;"
  ["Ö"]=>
  string(6) "&Ouml;"
  ["Φ"]=>
  string(5) "&Phi;"
  ["Π"]=>
  string(4) "&Pi;"
  ["″"]=>
  string(7) "&Prime;"
  ["Ψ"]=>
  string(5) "&Psi;"
  ["Ρ"]=>
  string(5) "&Rho;"
  ["Š"]=>
  string(8) "&Scaron;"
  ["Σ"]=>
  string(7) "&Sigma;"
  ["Þ"]=>
  string(7) "&THORN;"
  ["Τ"]=>
  string(5) "&Tau;"
  ["Θ"]=>
  string(7) "&Theta;"
  ["Ú"]=>
  string(8) "&Uacute;"
  ["Û"]=>
  string(7) "&Ucirc;"
  ["Ù"]=>
  string(8) "&Ugrave;"
  ["Υ"]=>
  string(9) "&Upsilon;"
  ["Ü"]=>
  string(6) "&Uuml;"
  ["Ξ"]=>
  string(4) "&Xi;"
  ["Ý"]=>
  string(8) "&Yacute;"
  ["Ÿ"]=>
  string(6) "&Yuml;"
  ["Ζ"]=>
  string(6) "&Zeta;"
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
  ["ℵ"]=>
  string(9) "&alefsym;"
  ["α"]=>
  string(7) "&alpha;"
  ["&"]=>
  string(5) "&amp;"
  ["∧"]=>
  string(5) "&and;"
  ["∠"]=>
  string(5) "&ang;"
  ["å"]=>
  string(7) "&aring;"
  ["≈"]=>
  string(7) "&asymp;"
  ["ã"]=>
  string(8) "&atilde;"
  ["ä"]=>
  string(6) "&auml;"
  ["„"]=>
  string(7) "&bdquo;"
  ["β"]=>
  string(6) "&beta;"
  ["¦"]=>
  string(8) "&brvbar;"
  ["•"]=>
  string(6) "&bull;"
  ["∩"]=>
  string(5) "&cap;"
  ["ç"]=>
  string(8) "&ccedil;"
  ["¸"]=>
  string(7) "&cedil;"
  ["¢"]=>
  string(6) "&cent;"
  ["χ"]=>
  string(5) "&chi;"
  ["ˆ"]=>
  string(6) "&circ;"
  ["♣"]=>
  string(7) "&clubs;"
  ["≅"]=>
  string(6) "&cong;"
  ["©"]=>
  string(6) "&copy;"
  ["↵"]=>
  string(7) "&crarr;"
  ["∪"]=>
  string(5) "&cup;"
  ["¤"]=>
  string(8) "&curren;"
  ["⇓"]=>
  string(6) "&dArr;"
  ["†"]=>
  string(8) "&dagger;"
  ["↓"]=>
  string(6) "&darr;"
  ["°"]=>
  string(5) "&deg;"
  ["δ"]=>
  string(7) "&delta;"
  ["♦"]=>
  string(7) "&diams;"
  ["÷"]=>
  string(8) "&divide;"
  ["é"]=>
  string(8) "&eacute;"
  ["ê"]=>
  string(7) "&ecirc;"
  ["è"]=>
  string(8) "&egrave;"
  ["∅"]=>
  string(7) "&empty;"
  [" "]=>
  string(6) "&emsp;"
  [" "]=>
  string(6) "&ensp;"
  ["ε"]=>
  string(9) "&epsilon;"
  ["≡"]=>
  string(7) "&equiv;"
  ["η"]=>
  string(5) "&eta;"
  ["ð"]=>
  string(5) "&eth;"
  ["ë"]=>
  string(6) "&euml;"
  ["€"]=>
  string(6) "&euro;"
  ["∃"]=>
  string(7) "&exist;"
  ["ƒ"]=>
  string(6) "&fnof;"
  ["∀"]=>
  string(8) "&forall;"
  ["½"]=>
  string(8) "&frac12;"
  ["¼"]=>
  string(8) "&frac14;"
  ["¾"]=>
  string(8) "&frac34;"
  ["⁄"]=>
  string(7) "&frasl;"
  ["γ"]=>
  string(7) "&gamma;"
  ["≥"]=>
  string(4) "&ge;"
  [">"]=>
  string(4) "&gt;"
  ["⇔"]=>
  string(6) "&hArr;"
  ["↔"]=>
  string(6) "&harr;"
  ["♥"]=>
  string(8) "&hearts;"
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
  ["ℑ"]=>
  string(7) "&image;"
  ["∞"]=>
  string(7) "&infin;"
  ["∫"]=>
  string(5) "&int;"
  ["ι"]=>
  string(6) "&iota;"
  ["¿"]=>
  string(8) "&iquest;"
  ["∈"]=>
  string(6) "&isin;"
  ["ï"]=>
  string(6) "&iuml;"
  ["κ"]=>
  string(7) "&kappa;"
  ["⇐"]=>
  string(6) "&lArr;"
  ["λ"]=>
  string(8) "&lambda;"
  ["〈"]=>
  string(6) "&lang;"
  ["«"]=>
  string(7) "&laquo;"
  ["←"]=>
  string(6) "&larr;"
  ["⌈"]=>
  string(7) "&lceil;"
  ["“"]=>
  string(7) "&ldquo;"
  ["≤"]=>
  string(4) "&le;"
  ["⌊"]=>
  string(8) "&lfloor;"
  ["∗"]=>
  string(8) "&lowast;"
  ["◊"]=>
  string(5) "&loz;"
  ["‎"]=>
  string(5) "&lrm;"
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
  ["−"]=>
  string(7) "&minus;"
  ["μ"]=>
  string(4) "&mu;"
  ["∇"]=>
  string(7) "&nabla;"
  [" "]=>
  string(6) "&nbsp;"
  ["–"]=>
  string(7) "&ndash;"
  ["≠"]=>
  string(4) "&ne;"
  ["∋"]=>
  string(4) "&ni;"
  ["¬"]=>
  string(5) "&not;"
  ["∉"]=>
  string(7) "&notin;"
  ["⊄"]=>
  string(6) "&nsub;"
  ["ñ"]=>
  string(8) "&ntilde;"
  ["ν"]=>
  string(4) "&nu;"
  ["ó"]=>
  string(8) "&oacute;"
  ["ô"]=>
  string(7) "&ocirc;"
  ["œ"]=>
  string(7) "&oelig;"
  ["ò"]=>
  string(8) "&ograve;"
  ["‾"]=>
  string(7) "&oline;"
  ["ω"]=>
  string(7) "&omega;"
  ["ο"]=>
  string(9) "&omicron;"
  ["⊕"]=>
  string(7) "&oplus;"
  ["∨"]=>
  string(4) "&or;"
  ["ª"]=>
  string(6) "&ordf;"
  ["º"]=>
  string(6) "&ordm;"
  ["ø"]=>
  string(8) "&oslash;"
  ["õ"]=>
  string(8) "&otilde;"
  ["⊗"]=>
  string(8) "&otimes;"
  ["ö"]=>
  string(6) "&ouml;"
  ["¶"]=>
  string(6) "&para;"
  ["∂"]=>
  string(6) "&part;"
  ["‰"]=>
  string(8) "&permil;"
  ["⊥"]=>
  string(6) "&perp;"
  ["φ"]=>
  string(5) "&phi;"
  ["π"]=>
  string(4) "&pi;"
  ["ϖ"]=>
  string(5) "&piv;"
  ["±"]=>
  string(8) "&plusmn;"
  ["£"]=>
  string(7) "&pound;"
  ["′"]=>
  string(7) "&prime;"
  ["∏"]=>
  string(6) "&prod;"
  ["∝"]=>
  string(6) "&prop;"
  ["ψ"]=>
  string(5) "&psi;"
  ["""]=>
  string(6) "&quot;"
  ["⇒"]=>
  string(6) "&rArr;"
  ["√"]=>
  string(7) "&radic;"
  ["〉"]=>
  string(6) "&rang;"
  ["»"]=>
  string(7) "&raquo;"
  ["→"]=>
  string(6) "&rarr;"
  ["⌉"]=>
  string(7) "&rceil;"
  ["”"]=>
  string(7) "&rdquo;"
  ["ℜ"]=>
  string(6) "&real;"
  ["®"]=>
  string(5) "&reg;"
  ["⌋"]=>
  string(8) "&rfloor;"
  ["ρ"]=>
  string(5) "&rho;"
  ["‏"]=>
  string(5) "&rlm;"
  ["›"]=>
  string(8) "&rsaquo;"
  ["’"]=>
  string(7) "&rsquo;"
  ["‚"]=>
  string(7) "&sbquo;"
  ["š"]=>
  string(8) "&scaron;"
  ["⋅"]=>
  string(6) "&sdot;"
  ["§"]=>
  string(6) "&sect;"
  ["­"]=>
  string(5) "&shy;"
  ["σ"]=>
  string(7) "&sigma;"
  ["ς"]=>
  string(8) "&sigmaf;"
  ["∼"]=>
  string(5) "&sim;"
  ["♠"]=>
  string(8) "&spades;"
  ["⊂"]=>
  string(5) "&sub;"
  ["⊆"]=>
  string(6) "&sube;"
  ["∑"]=>
  string(5) "&sum;"
  ["¹"]=>
  string(6) "&sup1;"
  ["²"]=>
  string(6) "&sup2;"
  ["³"]=>
  string(6) "&sup3;"
  ["⊃"]=>
  string(5) "&sup;"
  ["⊇"]=>
  string(6) "&supe;"
  ["ß"]=>
  string(7) "&szlig;"
  ["τ"]=>
  string(5) "&tau;"
  ["∴"]=>
  string(8) "&there4;"
  ["θ"]=>
  string(7) "&theta;"
  ["ϑ"]=>
  string(10) "&thetasym;"
  [" "]=>
  string(8) "&thinsp;"
  ["þ"]=>
  string(7) "&thorn;"
  ["˜"]=>
  string(7) "&tilde;"
  ["×"]=>
  string(7) "&times;"
  ["™"]=>
  string(7) "&trade;"
  ["⇑"]=>
  string(6) "&uArr;"
  ["ú"]=>
  string(8) "&uacute;"
  ["↑"]=>
  string(6) "&uarr;"
  ["û"]=>
  string(7) "&ucirc;"
  ["ù"]=>
  string(8) "&ugrave;"
  ["¨"]=>
  string(5) "&uml;"
  ["ϒ"]=>
  string(7) "&upsih;"
  ["υ"]=>
  string(9) "&upsilon;"
  ["ü"]=>
  string(6) "&uuml;"
  ["℘"]=>
  string(8) "&weierp;"
  ["ξ"]=>
  string(4) "&xi;"
  ["ý"]=>
  string(8) "&yacute;"
  ["¥"]=>
  string(5) "&yen;"
  ["ÿ"]=>
  string(6) "&yuml;"
  ["ζ"]=>
  string(6) "&zeta;"
  ["‍"]=>
  string(5) "&zwj;"
  ["‌"]=>
  string(6) "&zwnj;"
}
-- with table = HTML_ENTITIES & quote_style = ENT_QUOTES --
array(253) {
  ["'"]=>
  string(6) "&#039;"
  ["Æ"]=>
  string(7) "&AElig;"
  ["Á"]=>
  string(8) "&Aacute;"
  ["Â"]=>
  string(7) "&Acirc;"
  ["À"]=>
  string(8) "&Agrave;"
  ["Α"]=>
  string(7) "&Alpha;"
  ["Å"]=>
  string(7) "&Aring;"
  ["Ã"]=>
  string(8) "&Atilde;"
  ["Ä"]=>
  string(6) "&Auml;"
  ["Β"]=>
  string(6) "&Beta;"
  ["Ç"]=>
  string(8) "&Ccedil;"
  ["Χ"]=>
  string(5) "&Chi;"
  ["‡"]=>
  string(8) "&Dagger;"
  ["Δ"]=>
  string(7) "&Delta;"
  ["Ð"]=>
  string(5) "&ETH;"
  ["É"]=>
  string(8) "&Eacute;"
  ["Ê"]=>
  string(7) "&Ecirc;"
  ["È"]=>
  string(8) "&Egrave;"
  ["Ε"]=>
  string(9) "&Epsilon;"
  ["Η"]=>
  string(5) "&Eta;"
  ["Ë"]=>
  string(6) "&Euml;"
  ["Γ"]=>
  string(7) "&Gamma;"
  ["Í"]=>
  string(8) "&Iacute;"
  ["Î"]=>
  string(7) "&Icirc;"
  ["Ì"]=>
  string(8) "&Igrave;"
  ["Ι"]=>
  string(6) "&Iota;"
  ["Ï"]=>
  string(6) "&Iuml;"
  ["Κ"]=>
  string(7) "&Kappa;"
  ["Λ"]=>
  string(8) "&Lambda;"
  ["Μ"]=>
  string(4) "&Mu;"
  ["Ñ"]=>
  string(8) "&Ntilde;"
  ["Ν"]=>
  string(4) "&Nu;"
  ["Œ"]=>
  string(7) "&OElig;"
  ["Ó"]=>
  string(8) "&Oacute;"
  ["Ô"]=>
  string(7) "&Ocirc;"
  ["Ò"]=>
  string(8) "&Ograve;"
  ["Ω"]=>
  string(7) "&Omega;"
  ["Ο"]=>
  string(9) "&Omicron;"
  ["Ø"]=>
  string(8) "&Oslash;"
  ["Õ"]=>
  string(8) "&Otilde;"
  ["Ö"]=>
  string(6) "&Ouml;"
  ["Φ"]=>
  string(5) "&Phi;"
  ["Π"]=>
  string(4) "&Pi;"
  ["″"]=>
  string(7) "&Prime;"
  ["Ψ"]=>
  string(5) "&Psi;"
  ["Ρ"]=>
  string(5) "&Rho;"
  ["Š"]=>
  string(8) "&Scaron;"
  ["Σ"]=>
  string(7) "&Sigma;"
  ["Þ"]=>
  string(7) "&THORN;"
  ["Τ"]=>
  string(5) "&Tau;"
  ["Θ"]=>
  string(7) "&Theta;"
  ["Ú"]=>
  string(8) "&Uacute;"
  ["Û"]=>
  string(7) "&Ucirc;"
  ["Ù"]=>
  string(8) "&Ugrave;"
  ["Υ"]=>
  string(9) "&Upsilon;"
  ["Ü"]=>
  string(6) "&Uuml;"
  ["Ξ"]=>
  string(4) "&Xi;"
  ["Ý"]=>
  string(8) "&Yacute;"
  ["Ÿ"]=>
  string(6) "&Yuml;"
  ["Ζ"]=>
  string(6) "&Zeta;"
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
  ["ℵ"]=>
  string(9) "&alefsym;"
  ["α"]=>
  string(7) "&alpha;"
  ["&"]=>
  string(5) "&amp;"
  ["∧"]=>
  string(5) "&and;"
  ["∠"]=>
  string(5) "&ang;"
  ["å"]=>
  string(7) "&aring;"
  ["≈"]=>
  string(7) "&asymp;"
  ["ã"]=>
  string(8) "&atilde;"
  ["ä"]=>
  string(6) "&auml;"
  ["„"]=>
  string(7) "&bdquo;"
  ["β"]=>
  string(6) "&beta;"
  ["¦"]=>
  string(8) "&brvbar;"
  ["•"]=>
  string(6) "&bull;"
  ["∩"]=>
  string(5) "&cap;"
  ["ç"]=>
  string(8) "&ccedil;"
  ["¸"]=>
  string(7) "&cedil;"
  ["¢"]=>
  string(6) "&cent;"
  ["χ"]=>
  string(5) "&chi;"
  ["ˆ"]=>
  string(6) "&circ;"
  ["♣"]=>
  string(7) "&clubs;"
  ["≅"]=>
  string(6) "&cong;"
  ["©"]=>
  string(6) "&copy;"
  ["↵"]=>
  string(7) "&crarr;"
  ["∪"]=>
  string(5) "&cup;"
  ["¤"]=>
  string(8) "&curren;"
  ["⇓"]=>
  string(6) "&dArr;"
  ["†"]=>
  string(8) "&dagger;"
  ["↓"]=>
  string(6) "&darr;"
  ["°"]=>
  string(5) "&deg;"
  ["δ"]=>
  string(7) "&delta;"
  ["♦"]=>
  string(7) "&diams;"
  ["÷"]=>
  string(8) "&divide;"
  ["é"]=>
  string(8) "&eacute;"
  ["ê"]=>
  string(7) "&ecirc;"
  ["è"]=>
  string(8) "&egrave;"
  ["∅"]=>
  string(7) "&empty;"
  [" "]=>
  string(6) "&emsp;"
  [" "]=>
  string(6) "&ensp;"
  ["ε"]=>
  string(9) "&epsilon;"
  ["≡"]=>
  string(7) "&equiv;"
  ["η"]=>
  string(5) "&eta;"
  ["ð"]=>
  string(5) "&eth;"
  ["ë"]=>
  string(6) "&euml;"
  ["€"]=>
  string(6) "&euro;"
  ["∃"]=>
  string(7) "&exist;"
  ["ƒ"]=>
  string(6) "&fnof;"
  ["∀"]=>
  string(8) "&forall;"
  ["½"]=>
  string(8) "&frac12;"
  ["¼"]=>
  string(8) "&frac14;"
  ["¾"]=>
  string(8) "&frac34;"
  ["⁄"]=>
  string(7) "&frasl;"
  ["γ"]=>
  string(7) "&gamma;"
  ["≥"]=>
  string(4) "&ge;"
  [">"]=>
  string(4) "&gt;"
  ["⇔"]=>
  string(6) "&hArr;"
  ["↔"]=>
  string(6) "&harr;"
  ["♥"]=>
  string(8) "&hearts;"
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
  ["ℑ"]=>
  string(7) "&image;"
  ["∞"]=>
  string(7) "&infin;"
  ["∫"]=>
  string(5) "&int;"
  ["ι"]=>
  string(6) "&iota;"
  ["¿"]=>
  string(8) "&iquest;"
  ["∈"]=>
  string(6) "&isin;"
  ["ï"]=>
  string(6) "&iuml;"
  ["κ"]=>
  string(7) "&kappa;"
  ["⇐"]=>
  string(6) "&lArr;"
  ["λ"]=>
  string(8) "&lambda;"
  ["〈"]=>
  string(6) "&lang;"
  ["«"]=>
  string(7) "&laquo;"
  ["←"]=>
  string(6) "&larr;"
  ["⌈"]=>
  string(7) "&lceil;"
  ["“"]=>
  string(7) "&ldquo;"
  ["≤"]=>
  string(4) "&le;"
  ["⌊"]=>
  string(8) "&lfloor;"
  ["∗"]=>
  string(8) "&lowast;"
  ["◊"]=>
  string(5) "&loz;"
  ["‎"]=>
  string(5) "&lrm;"
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
  ["−"]=>
  string(7) "&minus;"
  ["μ"]=>
  string(4) "&mu;"
  ["∇"]=>
  string(7) "&nabla;"
  [" "]=>
  string(6) "&nbsp;"
  ["–"]=>
  string(7) "&ndash;"
  ["≠"]=>
  string(4) "&ne;"
  ["∋"]=>
  string(4) "&ni;"
  ["¬"]=>
  string(5) "&not;"
  ["∉"]=>
  string(7) "&notin;"
  ["⊄"]=>
  string(6) "&nsub;"
  ["ñ"]=>
  string(8) "&ntilde;"
  ["ν"]=>
  string(4) "&nu;"
  ["ó"]=>
  string(8) "&oacute;"
  ["ô"]=>
  string(7) "&ocirc;"
  ["œ"]=>
  string(7) "&oelig;"
  ["ò"]=>
  string(8) "&ograve;"
  ["‾"]=>
  string(7) "&oline;"
  ["ω"]=>
  string(7) "&omega;"
  ["ο"]=>
  string(9) "&omicron;"
  ["⊕"]=>
  string(7) "&oplus;"
  ["∨"]=>
  string(4) "&or;"
  ["ª"]=>
  string(6) "&ordf;"
  ["º"]=>
  string(6) "&ordm;"
  ["ø"]=>
  string(8) "&oslash;"
  ["õ"]=>
  string(8) "&otilde;"
  ["⊗"]=>
  string(8) "&otimes;"
  ["ö"]=>
  string(6) "&ouml;"
  ["¶"]=>
  string(6) "&para;"
  ["∂"]=>
  string(6) "&part;"
  ["‰"]=>
  string(8) "&permil;"
  ["⊥"]=>
  string(6) "&perp;"
  ["φ"]=>
  string(5) "&phi;"
  ["π"]=>
  string(4) "&pi;"
  ["ϖ"]=>
  string(5) "&piv;"
  ["±"]=>
  string(8) "&plusmn;"
  ["£"]=>
  string(7) "&pound;"
  ["′"]=>
  string(7) "&prime;"
  ["∏"]=>
  string(6) "&prod;"
  ["∝"]=>
  string(6) "&prop;"
  ["ψ"]=>
  string(5) "&psi;"
  ["""]=>
  string(6) "&quot;"
  ["⇒"]=>
  string(6) "&rArr;"
  ["√"]=>
  string(7) "&radic;"
  ["〉"]=>
  string(6) "&rang;"
  ["»"]=>
  string(7) "&raquo;"
  ["→"]=>
  string(6) "&rarr;"
  ["⌉"]=>
  string(7) "&rceil;"
  ["”"]=>
  string(7) "&rdquo;"
  ["ℜ"]=>
  string(6) "&real;"
  ["®"]=>
  string(5) "&reg;"
  ["⌋"]=>
  string(8) "&rfloor;"
  ["ρ"]=>
  string(5) "&rho;"
  ["‏"]=>
  string(5) "&rlm;"
  ["›"]=>
  string(8) "&rsaquo;"
  ["’"]=>
  string(7) "&rsquo;"
  ["‚"]=>
  string(7) "&sbquo;"
  ["š"]=>
  string(8) "&scaron;"
  ["⋅"]=>
  string(6) "&sdot;"
  ["§"]=>
  string(6) "&sect;"
  ["­"]=>
  string(5) "&shy;"
  ["σ"]=>
  string(7) "&sigma;"
  ["ς"]=>
  string(8) "&sigmaf;"
  ["∼"]=>
  string(5) "&sim;"
  ["♠"]=>
  string(8) "&spades;"
  ["⊂"]=>
  string(5) "&sub;"
  ["⊆"]=>
  string(6) "&sube;"
  ["∑"]=>
  string(5) "&sum;"
  ["¹"]=>
  string(6) "&sup1;"
  ["²"]=>
  string(6) "&sup2;"
  ["³"]=>
  string(6) "&sup3;"
  ["⊃"]=>
  string(5) "&sup;"
  ["⊇"]=>
  string(6) "&supe;"
  ["ß"]=>
  string(7) "&szlig;"
  ["τ"]=>
  string(5) "&tau;"
  ["∴"]=>
  string(8) "&there4;"
  ["θ"]=>
  string(7) "&theta;"
  ["ϑ"]=>
  string(10) "&thetasym;"
  [" "]=>
  string(8) "&thinsp;"
  ["þ"]=>
  string(7) "&thorn;"
  ["˜"]=>
  string(7) "&tilde;"
  ["×"]=>
  string(7) "&times;"
  ["™"]=>
  string(7) "&trade;"
  ["⇑"]=>
  string(6) "&uArr;"
  ["ú"]=>
  string(8) "&uacute;"
  ["↑"]=>
  string(6) "&uarr;"
  ["û"]=>
  string(7) "&ucirc;"
  ["ù"]=>
  string(8) "&ugrave;"
  ["¨"]=>
  string(5) "&uml;"
  ["ϒ"]=>
  string(7) "&upsih;"
  ["υ"]=>
  string(9) "&upsilon;"
  ["ü"]=>
  string(6) "&uuml;"
  ["℘"]=>
  string(8) "&weierp;"
  ["ξ"]=>
  string(4) "&xi;"
  ["ý"]=>
  string(8) "&yacute;"
  ["¥"]=>
  string(5) "&yen;"
  ["ÿ"]=>
  string(6) "&yuml;"
  ["ζ"]=>
  string(6) "&zeta;"
  ["‍"]=>
  string(5) "&zwj;"
  ["‌"]=>
  string(6) "&zwnj;"
}
-- with table = HTML_ENTITIES & quote_style = ENT_NOQUOTES --
array(251) {
  ["Æ"]=>
  string(7) "&AElig;"
  ["Á"]=>
  string(8) "&Aacute;"
  ["Â"]=>
  string(7) "&Acirc;"
  ["À"]=>
  string(8) "&Agrave;"
  ["Α"]=>
  string(7) "&Alpha;"
  ["Å"]=>
  string(7) "&Aring;"
  ["Ã"]=>
  string(8) "&Atilde;"
  ["Ä"]=>
  string(6) "&Auml;"
  ["Β"]=>
  string(6) "&Beta;"
  ["Ç"]=>
  string(8) "&Ccedil;"
  ["Χ"]=>
  string(5) "&Chi;"
  ["‡"]=>
  string(8) "&Dagger;"
  ["Δ"]=>
  string(7) "&Delta;"
  ["Ð"]=>
  string(5) "&ETH;"
  ["É"]=>
  string(8) "&Eacute;"
  ["Ê"]=>
  string(7) "&Ecirc;"
  ["È"]=>
  string(8) "&Egrave;"
  ["Ε"]=>
  string(9) "&Epsilon;"
  ["Η"]=>
  string(5) "&Eta;"
  ["Ë"]=>
  string(6) "&Euml;"
  ["Γ"]=>
  string(7) "&Gamma;"
  ["Í"]=>
  string(8) "&Iacute;"
  ["Î"]=>
  string(7) "&Icirc;"
  ["Ì"]=>
  string(8) "&Igrave;"
  ["Ι"]=>
  string(6) "&Iota;"
  ["Ï"]=>
  string(6) "&Iuml;"
  ["Κ"]=>
  string(7) "&Kappa;"
  ["Λ"]=>
  string(8) "&Lambda;"
  ["Μ"]=>
  string(4) "&Mu;"
  ["Ñ"]=>
  string(8) "&Ntilde;"
  ["Ν"]=>
  string(4) "&Nu;"
  ["Œ"]=>
  string(7) "&OElig;"
  ["Ó"]=>
  string(8) "&Oacute;"
  ["Ô"]=>
  string(7) "&Ocirc;"
  ["Ò"]=>
  string(8) "&Ograve;"
  ["Ω"]=>
  string(7) "&Omega;"
  ["Ο"]=>
  string(9) "&Omicron;"
  ["Ø"]=>
  string(8) "&Oslash;"
  ["Õ"]=>
  string(8) "&Otilde;"
  ["Ö"]=>
  string(6) "&Ouml;"
  ["Φ"]=>
  string(5) "&Phi;"
  ["Π"]=>
  string(4) "&Pi;"
  ["″"]=>
  string(7) "&Prime;"
  ["Ψ"]=>
  string(5) "&Psi;"
  ["Ρ"]=>
  string(5) "&Rho;"
  ["Š"]=>
  string(8) "&Scaron;"
  ["Σ"]=>
  string(7) "&Sigma;"
  ["Þ"]=>
  string(7) "&THORN;"
  ["Τ"]=>
  string(5) "&Tau;"
  ["Θ"]=>
  string(7) "&Theta;"
  ["Ú"]=>
  string(8) "&Uacute;"
  ["Û"]=>
  string(7) "&Ucirc;"
  ["Ù"]=>
  string(8) "&Ugrave;"
  ["Υ"]=>
  string(9) "&Upsilon;"
  ["Ü"]=>
  string(6) "&Uuml;"
  ["Ξ"]=>
  string(4) "&Xi;"
  ["Ý"]=>
  string(8) "&Yacute;"
  ["Ÿ"]=>
  string(6) "&Yuml;"
  ["Ζ"]=>
  string(6) "&Zeta;"
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
  ["ℵ"]=>
  string(9) "&alefsym;"
  ["α"]=>
  string(7) "&alpha;"
  ["&"]=>
  string(5) "&amp;"
  ["∧"]=>
  string(5) "&and;"
  ["∠"]=>
  string(5) "&ang;"
  ["å"]=>
  string(7) "&aring;"
  ["≈"]=>
  string(7) "&asymp;"
  ["ã"]=>
  string(8) "&atilde;"
  ["ä"]=>
  string(6) "&auml;"
  ["„"]=>
  string(7) "&bdquo;"
  ["β"]=>
  string(6) "&beta;"
  ["¦"]=>
  string(8) "&brvbar;"
  ["•"]=>
  string(6) "&bull;"
  ["∩"]=>
  string(5) "&cap;"
  ["ç"]=>
  string(8) "&ccedil;"
  ["¸"]=>
  string(7) "&cedil;"
  ["¢"]=>
  string(6) "&cent;"
  ["χ"]=>
  string(5) "&chi;"
  ["ˆ"]=>
  string(6) "&circ;"
  ["♣"]=>
  string(7) "&clubs;"
  ["≅"]=>
  string(6) "&cong;"
  ["©"]=>
  string(6) "&copy;"
  ["↵"]=>
  string(7) "&crarr;"
  ["∪"]=>
  string(5) "&cup;"
  ["¤"]=>
  string(8) "&curren;"
  ["⇓"]=>
  string(6) "&dArr;"
  ["†"]=>
  string(8) "&dagger;"
  ["↓"]=>
  string(6) "&darr;"
  ["°"]=>
  string(5) "&deg;"
  ["δ"]=>
  string(7) "&delta;"
  ["♦"]=>
  string(7) "&diams;"
  ["÷"]=>
  string(8) "&divide;"
  ["é"]=>
  string(8) "&eacute;"
  ["ê"]=>
  string(7) "&ecirc;"
  ["è"]=>
  string(8) "&egrave;"
  ["∅"]=>
  string(7) "&empty;"
  [" "]=>
  string(6) "&emsp;"
  [" "]=>
  string(6) "&ensp;"
  ["ε"]=>
  string(9) "&epsilon;"
  ["≡"]=>
  string(7) "&equiv;"
  ["η"]=>
  string(5) "&eta;"
  ["ð"]=>
  string(5) "&eth;"
  ["ë"]=>
  string(6) "&euml;"
  ["€"]=>
  string(6) "&euro;"
  ["∃"]=>
  string(7) "&exist;"
  ["ƒ"]=>
  string(6) "&fnof;"
  ["∀"]=>
  string(8) "&forall;"
  ["½"]=>
  string(8) "&frac12;"
  ["¼"]=>
  string(8) "&frac14;"
  ["¾"]=>
  string(8) "&frac34;"
  ["⁄"]=>
  string(7) "&frasl;"
  ["γ"]=>
  string(7) "&gamma;"
  ["≥"]=>
  string(4) "&ge;"
  [">"]=>
  string(4) "&gt;"
  ["⇔"]=>
  string(6) "&hArr;"
  ["↔"]=>
  string(6) "&harr;"
  ["♥"]=>
  string(8) "&hearts;"
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
  ["ℑ"]=>
  string(7) "&image;"
  ["∞"]=>
  string(7) "&infin;"
  ["∫"]=>
  string(5) "&int;"
  ["ι"]=>
  string(6) "&iota;"
  ["¿"]=>
  string(8) "&iquest;"
  ["∈"]=>
  string(6) "&isin;"
  ["ï"]=>
  string(6) "&iuml;"
  ["κ"]=>
  string(7) "&kappa;"
  ["⇐"]=>
  string(6) "&lArr;"
  ["λ"]=>
  string(8) "&lambda;"
  ["〈"]=>
  string(6) "&lang;"
  ["«"]=>
  string(7) "&laquo;"
  ["←"]=>
  string(6) "&larr;"
  ["⌈"]=>
  string(7) "&lceil;"
  ["“"]=>
  string(7) "&ldquo;"
  ["≤"]=>
  string(4) "&le;"
  ["⌊"]=>
  string(8) "&lfloor;"
  ["∗"]=>
  string(8) "&lowast;"
  ["◊"]=>
  string(5) "&loz;"
  ["‎"]=>
  string(5) "&lrm;"
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
  ["−"]=>
  string(7) "&minus;"
  ["μ"]=>
  string(4) "&mu;"
  ["∇"]=>
  string(7) "&nabla;"
  [" "]=>
  string(6) "&nbsp;"
  ["–"]=>
  string(7) "&ndash;"
  ["≠"]=>
  string(4) "&ne;"
  ["∋"]=>
  string(4) "&ni;"
  ["¬"]=>
  string(5) "&not;"
  ["∉"]=>
  string(7) "&notin;"
  ["⊄"]=>
  string(6) "&nsub;"
  ["ñ"]=>
  string(8) "&ntilde;"
  ["ν"]=>
  string(4) "&nu;"
  ["ó"]=>
  string(8) "&oacute;"
  ["ô"]=>
  string(7) "&ocirc;"
  ["œ"]=>
  string(7) "&oelig;"
  ["ò"]=>
  string(8) "&ograve;"
  ["‾"]=>
  string(7) "&oline;"
  ["ω"]=>
  string(7) "&omega;"
  ["ο"]=>
  string(9) "&omicron;"
  ["⊕"]=>
  string(7) "&oplus;"
  ["∨"]=>
  string(4) "&or;"
  ["ª"]=>
  string(6) "&ordf;"
  ["º"]=>
  string(6) "&ordm;"
  ["ø"]=>
  string(8) "&oslash;"
  ["õ"]=>
  string(8) "&otilde;"
  ["⊗"]=>
  string(8) "&otimes;"
  ["ö"]=>
  string(6) "&ouml;"
  ["¶"]=>
  string(6) "&para;"
  ["∂"]=>
  string(6) "&part;"
  ["‰"]=>
  string(8) "&permil;"
  ["⊥"]=>
  string(6) "&perp;"
  ["φ"]=>
  string(5) "&phi;"
  ["π"]=>
  string(4) "&pi;"
  ["ϖ"]=>
  string(5) "&piv;"
  ["±"]=>
  string(8) "&plusmn;"
  ["£"]=>
  string(7) "&pound;"
  ["′"]=>
  string(7) "&prime;"
  ["∏"]=>
  string(6) "&prod;"
  ["∝"]=>
  string(6) "&prop;"
  ["ψ"]=>
  string(5) "&psi;"
  ["⇒"]=>
  string(6) "&rArr;"
  ["√"]=>
  string(7) "&radic;"
  ["〉"]=>
  string(6) "&rang;"
  ["»"]=>
  string(7) "&raquo;"
  ["→"]=>
  string(6) "&rarr;"
  ["⌉"]=>
  string(7) "&rceil;"
  ["”"]=>
  string(7) "&rdquo;"
  ["ℜ"]=>
  string(6) "&real;"
  ["®"]=>
  string(5) "&reg;"
  ["⌋"]=>
  string(8) "&rfloor;"
  ["ρ"]=>
  string(5) "&rho;"
  ["‏"]=>
  string(5) "&rlm;"
  ["›"]=>
  string(8) "&rsaquo;"
  ["’"]=>
  string(7) "&rsquo;"
  ["‚"]=>
  string(7) "&sbquo;"
  ["š"]=>
  string(8) "&scaron;"
  ["⋅"]=>
  string(6) "&sdot;"
  ["§"]=>
  string(6) "&sect;"
  ["­"]=>
  string(5) "&shy;"
  ["σ"]=>
  string(7) "&sigma;"
  ["ς"]=>
  string(8) "&sigmaf;"
  ["∼"]=>
  string(5) "&sim;"
  ["♠"]=>
  string(8) "&spades;"
  ["⊂"]=>
  string(5) "&sub;"
  ["⊆"]=>
  string(6) "&sube;"
  ["∑"]=>
  string(5) "&sum;"
  ["¹"]=>
  string(6) "&sup1;"
  ["²"]=>
  string(6) "&sup2;"
  ["³"]=>
  string(6) "&sup3;"
  ["⊃"]=>
  string(5) "&sup;"
  ["⊇"]=>
  string(6) "&supe;"
  ["ß"]=>
  string(7) "&szlig;"
  ["τ"]=>
  string(5) "&tau;"
  ["∴"]=>
  string(8) "&there4;"
  ["θ"]=>
  string(7) "&theta;"
  ["ϑ"]=>
  string(10) "&thetasym;"
  [" "]=>
  string(8) "&thinsp;"
  ["þ"]=>
  string(7) "&thorn;"
  ["˜"]=>
  string(7) "&tilde;"
  ["×"]=>
  string(7) "&times;"
  ["™"]=>
  string(7) "&trade;"
  ["⇑"]=>
  string(6) "&uArr;"
  ["ú"]=>
  string(8) "&uacute;"
  ["↑"]=>
  string(6) "&uarr;"
  ["û"]=>
  string(7) "&ucirc;"
  ["ù"]=>
  string(8) "&ugrave;"
  ["¨"]=>
  string(5) "&uml;"
  ["ϒ"]=>
  string(7) "&upsih;"
  ["υ"]=>
  string(9) "&upsilon;"
  ["ü"]=>
  string(6) "&uuml;"
  ["℘"]=>
  string(8) "&weierp;"
  ["ξ"]=>
  string(4) "&xi;"
  ["ý"]=>
  string(8) "&yacute;"
  ["¥"]=>
  string(5) "&yen;"
  ["ÿ"]=>
  string(6) "&yuml;"
  ["ζ"]=>
  string(6) "&zeta;"
  ["‍"]=>
  string(5) "&zwj;"
  ["‌"]=>
  string(6) "&zwnj;"
}
Done
