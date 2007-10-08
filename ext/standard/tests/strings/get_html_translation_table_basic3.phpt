--TEST--
Test get_html_translation_table() function : basic functionality - table as HTML_ENTITIES & quote_style(ENT_NOQUOTES)
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) == "WIN"){
  die('skip Not for Windows');
}

if( ini_get("unicode.semantics") == "1")
  die('skip do not run when unicode on');

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

echo "-- with table = HTML_ENTITIES & quote_style = ENT_NOQUOTES --\n";
$table = HTML_ENTITIES;
$quote_style = ENT_NOQUOTES;
var_dump( get_html_translation_table($table, $quote_style) );


echo "Done\n";
?>
--EXPECTF--
*** Testing get_html_translation_table() : basic functionality ***
-- with table = HTML_ENTITIES & quote_style = ENT_NOQUOTES --
array(99) {
  ["�"]=>
  string(6) "&nbsp;"
  ["�"]=>
  string(7) "&iexcl;"
  ["�"]=>
  string(6) "&cent;"
  ["�"]=>
  string(7) "&pound;"
  ["�"]=>
  string(8) "&curren;"
  ["�"]=>
  string(5) "&yen;"
  ["�"]=>
  string(8) "&brvbar;"
  ["�"]=>
  string(6) "&sect;"
  ["�"]=>
  string(5) "&uml;"
  ["�"]=>
  string(6) "&copy;"
  ["�"]=>
  string(6) "&ordf;"
  ["�"]=>
  string(7) "&laquo;"
  ["�"]=>
  string(5) "&not;"
  ["�"]=>
  string(5) "&shy;"
  ["�"]=>
  string(5) "&reg;"
  ["�"]=>
  string(6) "&macr;"
  ["�"]=>
  string(5) "&deg;"
  ["�"]=>
  string(8) "&plusmn;"
  ["�"]=>
  string(6) "&sup2;"
  ["�"]=>
  string(6) "&sup3;"
  ["�"]=>
  string(7) "&acute;"
  ["�"]=>
  string(7) "&micro;"
  ["�"]=>
  string(6) "&para;"
  ["�"]=>
  string(8) "&middot;"
  ["�"]=>
  string(7) "&cedil;"
  ["�"]=>
  string(6) "&sup1;"
  ["�"]=>
  string(6) "&ordm;"
  ["�"]=>
  string(7) "&raquo;"
  ["�"]=>
  string(8) "&frac14;"
  ["�"]=>
  string(8) "&frac12;"
  ["�"]=>
  string(8) "&frac34;"
  ["�"]=>
  string(8) "&iquest;"
  ["�"]=>
  string(8) "&Agrave;"
  ["�"]=>
  string(8) "&Aacute;"
  ["�"]=>
  string(7) "&Acirc;"
  ["�"]=>
  string(8) "&Atilde;"
  ["�"]=>
  string(6) "&Auml;"
  ["�"]=>
  string(7) "&Aring;"
  ["�"]=>
  string(7) "&AElig;"
  ["�"]=>
  string(8) "&Ccedil;"
  ["�"]=>
  string(8) "&Egrave;"
  ["�"]=>
  string(8) "&Eacute;"
  ["�"]=>
  string(7) "&Ecirc;"
  ["�"]=>
  string(6) "&Euml;"
  ["�"]=>
  string(8) "&Igrave;"
  ["�"]=>
  string(8) "&Iacute;"
  ["�"]=>
  string(7) "&Icirc;"
  ["�"]=>
  string(6) "&Iuml;"
  ["�"]=>
  string(5) "&ETH;"
  ["�"]=>
  string(8) "&Ntilde;"
  ["�"]=>
  string(8) "&Ograve;"
  ["�"]=>
  string(8) "&Oacute;"
  ["�"]=>
  string(7) "&Ocirc;"
  ["�"]=>
  string(8) "&Otilde;"
  ["�"]=>
  string(6) "&Ouml;"
  ["�"]=>
  string(7) "&times;"
  ["�"]=>
  string(8) "&Oslash;"
  ["�"]=>
  string(8) "&Ugrave;"
  ["�"]=>
  string(8) "&Uacute;"
  ["�"]=>
  string(7) "&Ucirc;"
  ["�"]=>
  string(6) "&Uuml;"
  ["�"]=>
  string(8) "&Yacute;"
  ["�"]=>
  string(7) "&THORN;"
  ["�"]=>
  string(7) "&szlig;"
  ["�"]=>
  string(8) "&agrave;"
  ["�"]=>
  string(8) "&aacute;"
  ["�"]=>
  string(7) "&acirc;"
  ["�"]=>
  string(8) "&atilde;"
  ["�"]=>
  string(6) "&auml;"
  ["�"]=>
  string(7) "&aring;"
  ["�"]=>
  string(7) "&aelig;"
  ["�"]=>
  string(8) "&ccedil;"
  ["�"]=>
  string(8) "&egrave;"
  ["�"]=>
  string(8) "&eacute;"
  ["�"]=>
  string(7) "&ecirc;"
  ["�"]=>
  string(6) "&euml;"
  ["�"]=>
  string(8) "&igrave;"
  ["�"]=>
  string(8) "&iacute;"
  ["�"]=>
  string(7) "&icirc;"
  ["�"]=>
  string(6) "&iuml;"
  ["�"]=>
  string(5) "&eth;"
  ["�"]=>
  string(8) "&ntilde;"
  ["�"]=>
  string(8) "&ograve;"
  ["�"]=>
  string(8) "&oacute;"
  ["�"]=>
  string(7) "&ocirc;"
  ["�"]=>
  string(8) "&otilde;"
  ["�"]=>
  string(6) "&ouml;"
  ["�"]=>
  string(8) "&divide;"
  ["�"]=>
  string(8) "&oslash;"
  ["�"]=>
  string(8) "&ugrave;"
  ["�"]=>
  string(8) "&uacute;"
  ["�"]=>
  string(7) "&ucirc;"
  ["�"]=>
  string(6) "&uuml;"
  ["�"]=>
  string(8) "&yacute;"
  ["�"]=>
  string(7) "&thorn;"
  ["�"]=>
  string(6) "&yuml;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
  ["&"]=>
  string(5) "&amp;"
}
Done
--UEXPECTF--
*** Testing get_html_translation_table() : basic functionality ***
-- with table = HTML_ENTITIES & quote_style = ENT_NOQUOTES --
array(310) {
  [u" "]=>
  unicode(6) "&nbsp;"
  [u"¡"]=>
  unicode(7) "&iexcl;"
  [u"¢"]=>
  unicode(6) "&cent;"
  [u"£"]=>
  unicode(7) "&pound;"
  [u"¤"]=>
  unicode(8) "&curren;"
  [u"¥"]=>
  unicode(5) "&yen;"
  [u"¦"]=>
  unicode(8) "&brvbar;"
  [u"§"]=>
  unicode(6) "&sect;"
  [u"¨"]=>
  unicode(5) "&uml;"
  [u"©"]=>
  unicode(6) "&copy;"
  [u"ª"]=>
  unicode(6) "&ordf;"
  [u"«"]=>
  unicode(7) "&laquo;"
  [u"¬"]=>
  unicode(5) "&not;"
  [u"­"]=>
  unicode(5) "&shy;"
  [u"®"]=>
  unicode(5) "&reg;"
  [u"¯"]=>
  unicode(6) "&macr;"
  [u"°"]=>
  unicode(5) "&deg;"
  [u"±"]=>
  unicode(8) "&plusmn;"
  [u"²"]=>
  unicode(6) "&sup2;"
  [u"³"]=>
  unicode(6) "&sup3;"
  [u"´"]=>
  unicode(7) "&acute;"
  [u"µ"]=>
  unicode(7) "&micro;"
  [u"¶"]=>
  unicode(6) "&para;"
  [u"·"]=>
  unicode(8) "&middot;"
  [u"¸"]=>
  unicode(7) "&cedil;"
  [u"¹"]=>
  unicode(6) "&sup1;"
  [u"º"]=>
  unicode(6) "&ordm;"
  [u"»"]=>
  unicode(7) "&raquo;"
  [u"¼"]=>
  unicode(8) "&frac14;"
  [u"½"]=>
  unicode(8) "&frac12;"
  [u"¾"]=>
  unicode(8) "&frac34;"
  [u"¿"]=>
  unicode(8) "&iquest;"
  [u"À"]=>
  unicode(8) "&Agrave;"
  [u"Á"]=>
  unicode(8) "&Aacute;"
  [u"Â"]=>
  unicode(7) "&Acirc;"
  [u"Ã"]=>
  unicode(8) "&Atilde;"
  [u"Ä"]=>
  unicode(6) "&Auml;"
  [u"Å"]=>
  unicode(7) "&Aring;"
  [u"Æ"]=>
  unicode(7) "&AElig;"
  [u"Ç"]=>
  unicode(8) "&Ccedil;"
  [u"È"]=>
  unicode(8) "&Egrave;"
  [u"É"]=>
  unicode(8) "&Eacute;"
  [u"Ê"]=>
  unicode(7) "&Ecirc;"
  [u"Ë"]=>
  unicode(6) "&Euml;"
  [u"Ì"]=>
  unicode(8) "&Igrave;"
  [u"Í"]=>
  unicode(8) "&Iacute;"
  [u"Î"]=>
  unicode(7) "&Icirc;"
  [u"Ï"]=>
  unicode(6) "&Iuml;"
  [u"Ð"]=>
  unicode(5) "&ETH;"
  [u"Ñ"]=>
  unicode(8) "&Ntilde;"
  [u"Ò"]=>
  unicode(8) "&Ograve;"
  [u"Ó"]=>
  unicode(8) "&Oacute;"
  [u"Ô"]=>
  unicode(7) "&Ocirc;"
  [u"Õ"]=>
  unicode(8) "&Otilde;"
  [u"Ö"]=>
  unicode(6) "&Ouml;"
  [u"×"]=>
  unicode(7) "&times;"
  [u"Ø"]=>
  unicode(8) "&Oslash;"
  [u"Ù"]=>
  unicode(8) "&Ugrave;"
  [u"Ú"]=>
  unicode(8) "&Uacute;"
  [u"Û"]=>
  unicode(7) "&Ucirc;"
  [u"Ü"]=>
  unicode(6) "&Uuml;"
  [u"Ý"]=>
  unicode(8) "&Yacute;"
  [u"Þ"]=>
  unicode(7) "&THORN;"
  [u"ß"]=>
  unicode(7) "&szlig;"
  [u"à"]=>
  unicode(8) "&agrave;"
  [u"á"]=>
  unicode(8) "&aacute;"
  [u"â"]=>
  unicode(7) "&acirc;"
  [u"ã"]=>
  unicode(8) "&atilde;"
  [u"ä"]=>
  unicode(6) "&auml;"
  [u"å"]=>
  unicode(7) "&aring;"
  [u"æ"]=>
  unicode(7) "&aelig;"
  [u"ç"]=>
  unicode(8) "&ccedil;"
  [u"è"]=>
  unicode(8) "&egrave;"
  [u"é"]=>
  unicode(8) "&eacute;"
  [u"ê"]=>
  unicode(7) "&ecirc;"
  [u"ë"]=>
  unicode(6) "&euml;"
  [u"ì"]=>
  unicode(8) "&igrave;"
  [u"í"]=>
  unicode(8) "&iacute;"
  [u"î"]=>
  unicode(7) "&icirc;"
  [u"ï"]=>
  unicode(6) "&iuml;"
  [u"ð"]=>
  unicode(5) "&eth;"
  [u"ñ"]=>
  unicode(8) "&ntilde;"
  [u"ò"]=>
  unicode(8) "&ograve;"
  [u"ó"]=>
  unicode(8) "&oacute;"
  [u"ô"]=>
  unicode(7) "&ocirc;"
  [u"õ"]=>
  unicode(8) "&otilde;"
  [u"ö"]=>
  unicode(6) "&ouml;"
  [u"÷"]=>
  unicode(8) "&divide;"
  [u"ø"]=>
  unicode(8) "&oslash;"
  [u"ù"]=>
  unicode(8) "&ugrave;"
  [u"ú"]=>
  unicode(8) "&uacute;"
  [u"û"]=>
  unicode(7) "&ucirc;"
  [u"ü"]=>
  unicode(6) "&uuml;"
  [u"ý"]=>
  unicode(8) "&yacute;"
  [u"þ"]=>
  unicode(7) "&thorn;"
  [u"ÿ"]=>
  unicode(6) "&yuml;"
  [u"Œ"]=>
  unicode(7) "&OElig;"
  [u"œ"]=>
  unicode(7) "&oelig;"
  [u"Š"]=>
  unicode(8) "&Scaron;"
  [u"š"]=>
  unicode(8) "&scaron;"
  [u"Ÿ"]=>
  unicode(6) "&Yuml;"
  [u"ƒ"]=>
  unicode(6) "&fnof;"
  [u"ˆ"]=>
  unicode(6) "&circ;"
  [u"˜"]=>
  unicode(7) "&tilde;"
  [u"Α"]=>
  unicode(7) "&Alpha;"
  [u"Β"]=>
  unicode(6) "&Beta;"
  [u"Γ"]=>
  unicode(7) "&Gamma;"
  [u"Δ"]=>
  unicode(7) "&Delta;"
  [u"Ε"]=>
  unicode(9) "&Epsilon;"
  [u"Ζ"]=>
  unicode(6) "&Zeta;"
  [u"Η"]=>
  unicode(5) "&Eta;"
  [u"Θ"]=>
  unicode(7) "&Theta;"
  [u"Ι"]=>
  unicode(6) "&Iota;"
  [u"Κ"]=>
  unicode(7) "&Kappa;"
  [u"Λ"]=>
  unicode(8) "&Lambda;"
  [u"Μ"]=>
  unicode(4) "&Mu;"
  [u"Ν"]=>
  unicode(4) "&Nu;"
  [u"Ξ"]=>
  unicode(4) "&Xi;"
  [u"Ο"]=>
  unicode(9) "&Omicron;"
  [u"Π"]=>
  unicode(4) "&Pi;"
  [u"Ρ"]=>
  unicode(5) "&Rho;"
  [u"Σ"]=>
  unicode(7) "&Sigma;"
  [u"Τ"]=>
  unicode(5) "&Tau;"
  [u"Υ"]=>
  unicode(9) "&Upsilon;"
  [u"Φ"]=>
  unicode(5) "&Phi;"
  [u"Χ"]=>
  unicode(5) "&Chi;"
  [u"Ψ"]=>
  unicode(5) "&Psi;"
  [u"Ω"]=>
  unicode(7) "&Omega;"
  [u"α"]=>
  unicode(7) "&alpha;"
  [u"β"]=>
  unicode(6) "&beta;"
  [u"γ"]=>
  unicode(7) "&gamma;"
  [u"δ"]=>
  unicode(7) "&delta;"
  [u"ε"]=>
  unicode(9) "&epsilon;"
  [u"ζ"]=>
  unicode(6) "&zeta;"
  [u"η"]=>
  unicode(5) "&eta;"
  [u"θ"]=>
  unicode(7) "&theta;"
  [u"ι"]=>
  unicode(6) "&iota;"
  [u"κ"]=>
  unicode(7) "&kappa;"
  [u"λ"]=>
  unicode(8) "&lambda;"
  [u"μ"]=>
  unicode(4) "&mu;"
  [u"ν"]=>
  unicode(4) "&nu;"
  [u"ξ"]=>
  unicode(4) "&xi;"
  [u"ο"]=>
  unicode(9) "&omicron;"
  [u"π"]=>
  unicode(4) "&pi;"
  [u"ρ"]=>
  unicode(5) "&rho;"
  [u"ς"]=>
  unicode(8) "&sigmaf;"
  [u"σ"]=>
  unicode(7) "&sigma;"
  [u"τ"]=>
  unicode(5) "&tau;"
  [u"υ"]=>
  unicode(9) "&upsilon;"
  [u"φ"]=>
  unicode(5) "&phi;"
  [u"χ"]=>
  unicode(5) "&chi;"
  [u"ψ"]=>
  unicode(5) "&psi;"
  [u"ω"]=>
  unicode(7) "&omega;"
  [u"ϑ"]=>
  unicode(10) "&thetasym;"
  [u"ϒ"]=>
  unicode(7) "&upsih;"
  [u"ϖ"]=>
  unicode(5) "&piv;"
  [u" "]=>
  unicode(6) "&ensp;"
  [u" "]=>
  unicode(6) "&emsp;"
  [u" "]=>
  unicode(8) "&thinsp;"
  [u"‌"]=>
  unicode(6) "&zwnj;"
  [u"‍"]=>
  unicode(5) "&zwj;"
  [u"‎"]=>
  unicode(5) "&lrm;"
  [u"‏"]=>
  unicode(5) "&rlm;"
  [u"–"]=>
  unicode(7) "&ndash;"
  [u"—"]=>
  unicode(7) "&mdash;"
  [u"‘"]=>
  unicode(7) "&lsquo;"
  [u"’"]=>
  unicode(7) "&rsquo;"
  [u"‚"]=>
  unicode(7) "&sbquo;"
  [u"“"]=>
  unicode(7) "&ldquo;"
  [u"”"]=>
  unicode(7) "&rdquo;"
  [u"„"]=>
  unicode(7) "&bdquo;"
  [u"†"]=>
  unicode(8) "&dagger;"
  [u"‡"]=>
  unicode(8) "&Dagger;"
  [u"•"]=>
  unicode(6) "&bull;"
  [u"…"]=>
  unicode(8) "&hellip;"
  [u"‰"]=>
  unicode(8) "&permil;"
  [u"′"]=>
  unicode(7) "&prime;"
  [u"″"]=>
  unicode(7) "&Prime;"
  [u"‹"]=>
  unicode(8) "&lsaquo;"
  [u"›"]=>
  unicode(8) "&rsaquo;"
  [u"‾"]=>
  unicode(7) "&oline;"
  [u"⁄"]=>
  unicode(7) "&frasl;"
  [u"€"]=>
  unicode(6) "&euro;"
  [u"ℑ"]=>
  unicode(7) "&image;"
  [u"℘"]=>
  unicode(8) "&weierp;"
  [u"ℜ"]=>
  unicode(6) "&real;"
  [u"™"]=>
  unicode(7) "&trade;"
  [u"ℵ"]=>
  unicode(9) "&alefsym;"
  [u"←"]=>
  unicode(6) "&larr;"
  [u"↑"]=>
  unicode(6) "&uarr;"
  [u"→"]=>
  unicode(6) "&rarr;"
  [u"↓"]=>
  unicode(6) "&darr;"
  [u"↔"]=>
  unicode(6) "&harr;"
  [u"↵"]=>
  unicode(7) "&crarr;"
  [u"⇐"]=>
  unicode(6) "&lArr;"
  [u"⇑"]=>
  unicode(6) "&uArr;"
  [u"⇒"]=>
  unicode(6) "&rArr;"
  [u"⇓"]=>
  unicode(6) "&dArr;"
  [u"⇔"]=>
  unicode(6) "&hArr;"
  [u"⇕"]=>
  unicode(6) "&vArr;"
  [u"⇚"]=>
  unicode(7) "&lAarr;"
  [u"⇛"]=>
  unicode(7) "&rAarr;"
  [u"⇝"]=>
  unicode(7) "&rarrw;"
  [u"∀"]=>
  unicode(8) "&forall;"
  [u"∁"]=>
  unicode(6) "&comp;"
  [u"∂"]=>
  unicode(6) "&part;"
  [u"∃"]=>
  unicode(7) "&exist;"
  [u"∄"]=>
  unicode(8) "&nexist;"
  [u"∅"]=>
  unicode(7) "&empty;"
  [u"∇"]=>
  unicode(7) "&nabla;"
  [u"∈"]=>
  unicode(6) "&isin;"
  [u"∉"]=>
  unicode(7) "&notin;"
  [u"∊"]=>
  unicode(7) "&epsis;"
  [u"∋"]=>
  unicode(4) "&ni;"
  [u"∌"]=>
  unicode(7) "&notni;"
  [u"∍"]=>
  unicode(7) "&bepsi;"
  [u"∏"]=>
  unicode(6) "&prod;"
  [u"∐"]=>
  unicode(8) "&coprod;"
  [u"∑"]=>
  unicode(5) "&sum;"
  [u"−"]=>
  unicode(7) "&minus;"
  [u"∓"]=>
  unicode(8) "&mnplus;"
  [u"∔"]=>
  unicode(8) "&plusdo;"
  [u"∖"]=>
  unicode(7) "&setmn;"
  [u"∗"]=>
  unicode(8) "&lowast;"
  [u"∘"]=>
  unicode(8) "&compfn;"
  [u"√"]=>
  unicode(7) "&radic;"
  [u"∝"]=>
  unicode(6) "&prop;"
  [u"∞"]=>
  unicode(7) "&infin;"
  [u"∟"]=>
  unicode(7) "&ang90;"
  [u"∠"]=>
  unicode(5) "&ang;"
  [u"∡"]=>
  unicode(8) "&angmsd;"
  [u"∢"]=>
  unicode(8) "&angsph;"
  [u"∣"]=>
  unicode(5) "&mid;"
  [u"∤"]=>
  unicode(6) "&nmid;"
  [u"∥"]=>
  unicode(5) "&par;"
  [u"∦"]=>
  unicode(6) "&npar;"
  [u"∧"]=>
  unicode(5) "&and;"
  [u"∨"]=>
  unicode(4) "&or;"
  [u"∩"]=>
  unicode(5) "&cap;"
  [u"∪"]=>
  unicode(5) "&cup;"
  [u"∫"]=>
  unicode(5) "&int;"
  [u"∮"]=>
  unicode(8) "&conint;"
  [u"∴"]=>
  unicode(8) "&there4;"
  [u"∵"]=>
  unicode(8) "&becaus;"
  [u"∼"]=>
  unicode(5) "&sim;"
  [u"∽"]=>
  unicode(6) "&bsim;"
  [u"≀"]=>
  unicode(8) "&wreath;"
  [u"≁"]=>
  unicode(6) "&nsim;"
  [u"≃"]=>
  unicode(6) "&sime;"
  [u"≄"]=>
  unicode(7) "&nsime;"
  [u"≅"]=>
  unicode(6) "&cong;"
  [u"≇"]=>
  unicode(7) "&ncong;"
  [u"≈"]=>
  unicode(7) "&asymp;"
  [u"≉"]=>
  unicode(5) "&nap;"
  [u"≊"]=>
  unicode(5) "&ape;"
  [u"≌"]=>
  unicode(7) "&bcong;"
  [u"≍"]=>
  unicode(7) "&asymp;"
  [u"≎"]=>
  unicode(6) "&bump;"
  [u"≏"]=>
  unicode(7) "&bumpe;"
  [u"≠"]=>
  unicode(4) "&ne;"
  [u"≡"]=>
  unicode(7) "&equiv;"
  [u"≤"]=>
  unicode(4) "&le;"
  [u"≥"]=>
  unicode(4) "&ge;"
  [u"≦"]=>
  unicode(4) "&lE;"
  [u"≧"]=>
  unicode(4) "&gE;"
  [u"≨"]=>
  unicode(5) "&lnE;"
  [u"≩"]=>
  unicode(5) "&gnE;"
  [u"≪"]=>
  unicode(4) "&Lt;"
  [u"≫"]=>
  unicode(4) "&Gt;"
  [u"≬"]=>
  unicode(7) "&twixt;"
  [u"≮"]=>
  unicode(5) "&nlt;"
  [u"≯"]=>
  unicode(5) "&ngt;"
  [u"≰"]=>
  unicode(6) "&nles;"
  [u"≱"]=>
  unicode(6) "&nges;"
  [u"≲"]=>
  unicode(6) "&lsim;"
  [u"≳"]=>
  unicode(6) "&gsim;"
  [u"≶"]=>
  unicode(4) "&lg;"
  [u"≷"]=>
  unicode(4) "&gl;"
  [u"≺"]=>
  unicode(4) "&pr;"
  [u"≻"]=>
  unicode(4) "&sc;"
  [u"≼"]=>
  unicode(7) "&cupre;"
  [u"≽"]=>
  unicode(7) "&sscue;"
  [u"≾"]=>
  unicode(7) "&prsim;"
  [u"≿"]=>
  unicode(7) "&scsim;"
  [u"⊀"]=>
  unicode(5) "&npr;"
  [u"⊁"]=>
  unicode(5) "&nsc;"
  [u"⊂"]=>
  unicode(5) "&sub;"
  [u"⊃"]=>
  unicode(5) "&sup;"
  [u"⊄"]=>
  unicode(6) "&nsub;"
  [u"⊅"]=>
  unicode(6) "&nsup;"
  [u"⊆"]=>
  unicode(6) "&sube;"
  [u"⊇"]=>
  unicode(6) "&supe;"
  [u"⊕"]=>
  unicode(7) "&oplus;"
  [u"⊗"]=>
  unicode(8) "&otimes;"
  [u"⊥"]=>
  unicode(6) "&perp;"
  [u"⋅"]=>
  unicode(6) "&sdot;"
  [u"⌈"]=>
  unicode(7) "&lceil;"
  [u"⌉"]=>
  unicode(7) "&rceil;"
  [u"⌊"]=>
  unicode(8) "&lfloor;"
  [u"⌋"]=>
  unicode(8) "&rfloor;"
  [u"〈"]=>
  unicode(6) "&lang;"
  [u"〉"]=>
  unicode(6) "&rang;"
  [u"◊"]=>
  unicode(5) "&loz;"
  [u"♠"]=>
  unicode(8) "&spades;"
  [u"♣"]=>
  unicode(7) "&clubs;"
  [u"♥"]=>
  unicode(8) "&hearts;"
  [u"♦"]=>
  unicode(7) "&diams;"
  [u"<"]=>
  unicode(4) "&lt;"
  [u">"]=>
  unicode(4) "&gt;"
  [u"&"]=>
  unicode(5) "&amp;"
}
Done
