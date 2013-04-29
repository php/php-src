--TEST--
Test get_html_translation_table() function : basic functionality - XHTML 1.0
--FILE--
<?php
echo "*** Testing get_html_translation_table() : basic functionality/XHTML 1.0 ***\n";

echo "-- with table = HTML_ENTITIES, ENT_QUOTES --\n";
$table = HTML_ENTITIES;
/* uses &#039; to share the code path with HTML 4.01 */
$tt = get_html_translation_table($table, ENT_QUOTES | ENT_XHTML, "UTF-8");
asort( $tt );
var_dump( count($tt) );
print_r( $tt );

echo "-- with table = HTML_ENTITIES, ENT_COMPAT --\n";
$table = HTML_ENTITIES;
$tt = get_html_translation_table($table, ENT_COMPAT | ENT_XHTML, "UTF-8");
var_dump( count($tt) );

echo "-- with table = HTML_ENTITIES, ENT_NOQUOTES --\n";
$table = HTML_ENTITIES;
$tt = get_html_translation_table($table, ENT_NOQUOTES | ENT_XHTML, "UTF-8");
var_dump( count($tt) );

echo "-- with table = HTML_SPECIALCHARS, ENT_COMPAT --\n";
$table = HTML_SPECIALCHARS; 
$tt = get_html_translation_table($table, ENT_COMPAT, "UTF-8");
asort( $tt );
var_dump( count($tt) );
print_r( $tt );

echo "-- with table = HTML_SPECIALCHARS, ENT_QUOTES --\n";
$table = HTML_SPECIALCHARS;
$tt = get_html_translation_table($table, ENT_QUOTES | ENT_XHTML, "UTF-8");
asort( $tt );
var_dump( $tt );

echo "-- with table = HTML_SPECIALCHARS, ENT_NOQUOTES --\n";
$table = HTML_SPECIALCHARS;
$tt = get_html_translation_table($table, ENT_NOQUOTES | ENT_XHTML, "UTF-8");
asort( $tt );
var_dump( $tt );


echo "Done\n";
?>
--EXPECT--
*** Testing get_html_translation_table() : basic functionality/XHTML 1.0 ***
-- with table = HTML_ENTITIES, ENT_QUOTES --
int(253)
Array
(
    ['] => &#039;
    [Æ] => &AElig;
    [Á] => &Aacute;
    [Â] => &Acirc;
    [À] => &Agrave;
    [Α] => &Alpha;
    [Å] => &Aring;
    [Ã] => &Atilde;
    [Ä] => &Auml;
    [Β] => &Beta;
    [Ç] => &Ccedil;
    [Χ] => &Chi;
    [‡] => &Dagger;
    [Δ] => &Delta;
    [Ð] => &ETH;
    [É] => &Eacute;
    [Ê] => &Ecirc;
    [È] => &Egrave;
    [Ε] => &Epsilon;
    [Η] => &Eta;
    [Ë] => &Euml;
    [Γ] => &Gamma;
    [Í] => &Iacute;
    [Î] => &Icirc;
    [Ì] => &Igrave;
    [Ι] => &Iota;
    [Ï] => &Iuml;
    [Κ] => &Kappa;
    [Λ] => &Lambda;
    [Μ] => &Mu;
    [Ñ] => &Ntilde;
    [Ν] => &Nu;
    [Œ] => &OElig;
    [Ó] => &Oacute;
    [Ô] => &Ocirc;
    [Ò] => &Ograve;
    [Ω] => &Omega;
    [Ο] => &Omicron;
    [Ø] => &Oslash;
    [Õ] => &Otilde;
    [Ö] => &Ouml;
    [Φ] => &Phi;
    [Π] => &Pi;
    [″] => &Prime;
    [Ψ] => &Psi;
    [Ρ] => &Rho;
    [Š] => &Scaron;
    [Σ] => &Sigma;
    [Þ] => &THORN;
    [Τ] => &Tau;
    [Θ] => &Theta;
    [Ú] => &Uacute;
    [Û] => &Ucirc;
    [Ù] => &Ugrave;
    [Υ] => &Upsilon;
    [Ü] => &Uuml;
    [Ξ] => &Xi;
    [Ý] => &Yacute;
    [Ÿ] => &Yuml;
    [Ζ] => &Zeta;
    [á] => &aacute;
    [â] => &acirc;
    [´] => &acute;
    [æ] => &aelig;
    [à] => &agrave;
    [ℵ] => &alefsym;
    [α] => &alpha;
    [&] => &amp;
    [∧] => &and;
    [∠] => &ang;
    [å] => &aring;
    [≈] => &asymp;
    [ã] => &atilde;
    [ä] => &auml;
    [„] => &bdquo;
    [β] => &beta;
    [¦] => &brvbar;
    [•] => &bull;
    [∩] => &cap;
    [ç] => &ccedil;
    [¸] => &cedil;
    [¢] => &cent;
    [χ] => &chi;
    [ˆ] => &circ;
    [♣] => &clubs;
    [≅] => &cong;
    [©] => &copy;
    [↵] => &crarr;
    [∪] => &cup;
    [¤] => &curren;
    [⇓] => &dArr;
    [†] => &dagger;
    [↓] => &darr;
    [°] => &deg;
    [δ] => &delta;
    [♦] => &diams;
    [÷] => &divide;
    [é] => &eacute;
    [ê] => &ecirc;
    [è] => &egrave;
    [∅] => &empty;
    [ ] => &emsp;
    [ ] => &ensp;
    [ε] => &epsilon;
    [≡] => &equiv;
    [η] => &eta;
    [ð] => &eth;
    [ë] => &euml;
    [€] => &euro;
    [∃] => &exist;
    [ƒ] => &fnof;
    [∀] => &forall;
    [½] => &frac12;
    [¼] => &frac14;
    [¾] => &frac34;
    [⁄] => &frasl;
    [γ] => &gamma;
    [≥] => &ge;
    [>] => &gt;
    [⇔] => &hArr;
    [↔] => &harr;
    [♥] => &hearts;
    […] => &hellip;
    [í] => &iacute;
    [î] => &icirc;
    [¡] => &iexcl;
    [ì] => &igrave;
    [ℑ] => &image;
    [∞] => &infin;
    [∫] => &int;
    [ι] => &iota;
    [¿] => &iquest;
    [∈] => &isin;
    [ï] => &iuml;
    [κ] => &kappa;
    [⇐] => &lArr;
    [λ] => &lambda;
    [〈] => &lang;
    [«] => &laquo;
    [←] => &larr;
    [⌈] => &lceil;
    [“] => &ldquo;
    [≤] => &le;
    [⌊] => &lfloor;
    [∗] => &lowast;
    [◊] => &loz;
    [‎] => &lrm;
    [‹] => &lsaquo;
    [‘] => &lsquo;
    [<] => &lt;
    [¯] => &macr;
    [—] => &mdash;
    [µ] => &micro;
    [·] => &middot;
    [−] => &minus;
    [μ] => &mu;
    [∇] => &nabla;
    [ ] => &nbsp;
    [–] => &ndash;
    [≠] => &ne;
    [∋] => &ni;
    [¬] => &not;
    [∉] => &notin;
    [⊄] => &nsub;
    [ñ] => &ntilde;
    [ν] => &nu;
    [ó] => &oacute;
    [ô] => &ocirc;
    [œ] => &oelig;
    [ò] => &ograve;
    [‾] => &oline;
    [ω] => &omega;
    [ο] => &omicron;
    [⊕] => &oplus;
    [∨] => &or;
    [ª] => &ordf;
    [º] => &ordm;
    [ø] => &oslash;
    [õ] => &otilde;
    [⊗] => &otimes;
    [ö] => &ouml;
    [¶] => &para;
    [∂] => &part;
    [‰] => &permil;
    [⊥] => &perp;
    [φ] => &phi;
    [π] => &pi;
    [ϖ] => &piv;
    [±] => &plusmn;
    [£] => &pound;
    [′] => &prime;
    [∏] => &prod;
    [∝] => &prop;
    [ψ] => &psi;
    ["] => &quot;
    [⇒] => &rArr;
    [√] => &radic;
    [〉] => &rang;
    [»] => &raquo;
    [→] => &rarr;
    [⌉] => &rceil;
    [”] => &rdquo;
    [ℜ] => &real;
    [®] => &reg;
    [⌋] => &rfloor;
    [ρ] => &rho;
    [‏] => &rlm;
    [›] => &rsaquo;
    [’] => &rsquo;
    [‚] => &sbquo;
    [š] => &scaron;
    [⋅] => &sdot;
    [§] => &sect;
    [­] => &shy;
    [σ] => &sigma;
    [ς] => &sigmaf;
    [∼] => &sim;
    [♠] => &spades;
    [⊂] => &sub;
    [⊆] => &sube;
    [∑] => &sum;
    [¹] => &sup1;
    [²] => &sup2;
    [³] => &sup3;
    [⊃] => &sup;
    [⊇] => &supe;
    [ß] => &szlig;
    [τ] => &tau;
    [∴] => &there4;
    [θ] => &theta;
    [ϑ] => &thetasym;
    [ ] => &thinsp;
    [þ] => &thorn;
    [˜] => &tilde;
    [×] => &times;
    [™] => &trade;
    [⇑] => &uArr;
    [ú] => &uacute;
    [↑] => &uarr;
    [û] => &ucirc;
    [ù] => &ugrave;
    [¨] => &uml;
    [ϒ] => &upsih;
    [υ] => &upsilon;
    [ü] => &uuml;
    [℘] => &weierp;
    [ξ] => &xi;
    [ý] => &yacute;
    [¥] => &yen;
    [ÿ] => &yuml;
    [ζ] => &zeta;
    [‍] => &zwj;
    [‌] => &zwnj;
)
-- with table = HTML_ENTITIES, ENT_COMPAT --
int(252)
-- with table = HTML_ENTITIES, ENT_NOQUOTES --
int(251)
-- with table = HTML_SPECIALCHARS, ENT_COMPAT --
int(4)
Array
(
    [&] => &amp;
    [>] => &gt;
    [<] => &lt;
    ["] => &quot;
)
-- with table = HTML_SPECIALCHARS, ENT_QUOTES --
array(5) {
  ["&"]=>
  string(5) "&amp;"
  ["'"]=>
  string(6) "&apos;"
  [">"]=>
  string(4) "&gt;"
  ["<"]=>
  string(4) "&lt;"
  ["""]=>
  string(6) "&quot;"
}
-- with table = HTML_SPECIALCHARS, ENT_NOQUOTES --
array(3) {
  ["&"]=>
  string(5) "&amp;"
  [">"]=>
  string(4) "&gt;"
  ["<"]=>
  string(4) "&lt;"
}
Done