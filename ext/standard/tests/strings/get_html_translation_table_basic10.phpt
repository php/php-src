--TEST--
Test get_html_translation_table() function: htmlentities/HTML 4/ISO-8859-1 (bug #64011)
--FILE--
<?php

function so($a,$b) { return ord($a) - ord($b); }

$table = HTML_ENTITIES;
$tt = get_html_translation_table($table, ENT_COMPAT, "ISO-8859-1");
uksort( $tt, 'so' );
var_dump( count($tt) );
print_r( $tt );
echo "Done\n";

?>
--EXPECT--
int(100)
Array
(
    ["] => &quot;
    [&] => &amp;
    [<] => &lt;
    [>] => &gt;
    [ ] => &nbsp;
    [¡] => &iexcl;
    [¢] => &cent;
    [£] => &pound;
    [¤] => &curren;
    [¥] => &yen;
    [¦] => &brvbar;
    [§] => &sect;
    [¨] => &uml;
    [©] => &copy;
    [ª] => &ordf;
    [«] => &laquo;
    [¬] => &not;
    [­] => &shy;
    [®] => &reg;
    [¯] => &macr;
    [°] => &deg;
    [±] => &plusmn;
    [²] => &sup2;
    [³] => &sup3;
    [´] => &acute;
    [µ] => &micro;
    [¶] => &para;
    [·] => &middot;
    [¸] => &cedil;
    [¹] => &sup1;
    [º] => &ordm;
    [»] => &raquo;
    [¼] => &frac14;
    [½] => &frac12;
    [¾] => &frac34;
    [¿] => &iquest;
    [À] => &Agrave;
    [Á] => &Aacute;
    [Â] => &Acirc;
    [Ã] => &Atilde;
    [Ä] => &Auml;
    [Å] => &Aring;
    [Æ] => &AElig;
    [Ç] => &Ccedil;
    [È] => &Egrave;
    [É] => &Eacute;
    [Ê] => &Ecirc;
    [Ë] => &Euml;
    [Ì] => &Igrave;
    [Í] => &Iacute;
    [Î] => &Icirc;
    [Ï] => &Iuml;
    [Ð] => &ETH;
    [Ñ] => &Ntilde;
    [Ò] => &Ograve;
    [Ó] => &Oacute;
    [Ô] => &Ocirc;
    [Õ] => &Otilde;
    [Ö] => &Ouml;
    [×] => &times;
    [Ø] => &Oslash;
    [Ù] => &Ugrave;
    [Ú] => &Uacute;
    [Û] => &Ucirc;
    [Ü] => &Uuml;
    [Ý] => &Yacute;
    [Þ] => &THORN;
    [ß] => &szlig;
    [à] => &agrave;
    [á] => &aacute;
    [â] => &acirc;
    [ã] => &atilde;
    [ä] => &auml;
    [å] => &aring;
    [æ] => &aelig;
    [ç] => &ccedil;
    [è] => &egrave;
    [é] => &eacute;
    [ê] => &ecirc;
    [ë] => &euml;
    [ì] => &igrave;
    [í] => &iacute;
    [î] => &icirc;
    [ï] => &iuml;
    [ð] => &eth;
    [ñ] => &ntilde;
    [ò] => &ograve;
    [ó] => &oacute;
    [ô] => &ocirc;
    [õ] => &otilde;
    [ö] => &ouml;
    [÷] => &divide;
    [ø] => &oslash;
    [ù] => &ugrave;
    [ú] => &uacute;
    [û] => &ucirc;
    [ü] => &uuml;
    [ý] => &yacute;
    [þ] => &thorn;
    [ÿ] => &yuml;
)
Done
