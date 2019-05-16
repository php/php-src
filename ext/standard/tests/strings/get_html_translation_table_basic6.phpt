--TEST--
Test get_html_translation_table() function : basic functionality - HTML 5/Windows-1251
--FILE--
<?php

function so($a,$b) { return ord($a) - ord($b); }

echo "*** Testing get_html_translation_table() : basic functionality - HTML 5/Windows-1251 ***\n";

echo "-- with table = HTML_ENTITIES, ENT_COMPAT --\n";
$table = HTML_ENTITIES;
$tt = get_html_translation_table($table, ENT_COMPAT | ENT_HTML5, "Windows-1251");
uksort( $tt, 'so' );
var_dump( count($tt) );
print_r( $tt );

echo "-- with table = HTML_ENTITIES, ENT_QUOTES --\n";
$table = HTML_ENTITIES;
$tt = get_html_translation_table($table, ENT_QUOTES | ENT_HTML5, "Windows-1251");
var_dump( count($tt) );

echo "-- with table = HTML_ENTITIES, ENT_NOQUOTES --\n";
$table = HTML_ENTITIES;
$tt = get_html_translation_table($table, ENT_NOQUOTES | ENT_HTML5, "Windows-1251");
var_dump( count($tt) );

echo "-- with table = HTML_SPECIALCHARS, ENT_COMPAT --\n";
$table = HTML_SPECIALCHARS;
$tt = get_html_translation_table($table, ENT_COMPAT, "Windows-1251");
uksort( $tt, 'so' );
var_dump( count($tt) );
print_r( $tt );

echo "-- with table = HTML_SPECIALCHARS, ENT_QUOTES --\n";
$table = HTML_SPECIALCHARS;
$tt = get_html_translation_table($table, ENT_QUOTES | ENT_HTML5, "Windows-1251");
uksort( $tt, 'so' );
var_dump( $tt );

echo "-- with table = HTML_SPECIALCHARS, ENT_NOQUOTES --\n";
$table = HTML_SPECIALCHARS;
$tt = get_html_translation_table($table, ENT_NOQUOTES | ENT_HTML5, "Windows-1251");
uasort( $tt, 'so' );
var_dump( $tt );


echo "Done\n";
?>
--EXPECT--
*** Testing get_html_translation_table() : basic functionality - HTML 5/Windows-1251 ***
-- with table = HTML_ENTITIES, ENT_COMPAT --
int(157)
Array
(
    [	] => &Tab;
    [
] => &NewLine;
    [!] => &excl;
    ["] => &quot;
    [#] => &num;
    [$] => &dollar;
    [%] => &percnt;
    [&] => &amp;
    [(] => &lpar;
    [)] => &rpar;
    [*] => &ast;
    [+] => &plus;
    [,] => &comma;
    [.] => &period;
    [/] => &sol;
    [:] => &colon;
    [;] => &semi;
    [<] => &lt;
    [=] => &equals;
    [>] => &gt;
    [?] => &quest;
    [@] => &commat;
    [[] => &lbrack;
    [\] => &bsol;
    []] => &rsqb;
    [^] => &Hat;
    [_] => &lowbar;
    [`] => &grave;
    [fj] => &fjlig;
    [{] => &lbrace;
    [|] => &vert;
    [}] => &rcub;
    [€] => &DJcy;
    [] => &GJcy;
    [‚] => &sbquo;
    [ƒ] => &gjcy;
    [„] => &bdquo;
    […] => &hellip;
    [†] => &dagger;
    [‡] => &Dagger;
    [ˆ] => &euro;
    [‰] => &permil;
    [Š] => &LJcy;
    [‹] => &lsaquo;
    [Œ] => &NJcy;
    [] => &KJcy;
    [Ž] => &TSHcy;
    [] => &DZcy;
    [] => &djcy;
    [‘] => &OpenCurlyQuote;
    [’] => &rsquo;
    [“] => &OpenCurlyDoubleQuote;
    [”] => &rdquo;
    [•] => &bull;
    [–] => &ndash;
    [—] => &mdash;
    [™] => &trade;
    [š] => &ljcy;
    [›] => &rsaquo;
    [œ] => &njcy;
    [] => &kjcy;
    [ž] => &tshcy;
    [Ÿ] => &dzcy;
    [ ] => &nbsp;
    [¡] => &Ubrcy;
    [¢] => &ubrcy;
    [£] => &Jsercy;
    [¤] => &curren;
    [¦] => &brvbar;
    [§] => &sect;
    [¨] => &IOcy;
    [©] => &copy;
    [ª] => &Jukcy;
    [«] => &laquo;
    [¬] => &not;
    [­] => &shy;
    [®] => &reg;
    [¯] => &YIcy;
    [°] => &deg;
    [±] => &plusmn;
    [²] => &Iukcy;
    [³] => &iukcy;
    [µ] => &micro;
    [¶] => &para;
    [·] => &CenterDot;
    [¸] => &iocy;
    [¹] => &numero;
    [º] => &jukcy;
    [»] => &raquo;
    [¼] => &jsercy;
    [½] => &DScy;
    [¾] => &dscy;
    [¿] => &yicy;
    [À] => &Acy;
    [Á] => &Bcy;
    [Â] => &Vcy;
    [Ã] => &Gcy;
    [Ä] => &Dcy;
    [Å] => &IEcy;
    [Æ] => &ZHcy;
    [Ç] => &Zcy;
    [È] => &Icy;
    [É] => &Jcy;
    [Ê] => &Kcy;
    [Ë] => &Lcy;
    [Ì] => &Mcy;
    [Í] => &Ncy;
    [Î] => &Ocy;
    [Ï] => &Pcy;
    [Ð] => &Rcy;
    [Ñ] => &Scy;
    [Ò] => &Tcy;
    [Ó] => &Ucy;
    [Ô] => &Fcy;
    [Õ] => &KHcy;
    [Ö] => &TScy;
    [×] => &CHcy;
    [Ø] => &SHcy;
    [Ù] => &SHCHcy;
    [Ú] => &HARDcy;
    [Û] => &Ycy;
    [Ü] => &SOFTcy;
    [Ý] => &Ecy;
    [Þ] => &YUcy;
    [ß] => &YAcy;
    [à] => &acy;
    [á] => &bcy;
    [â] => &vcy;
    [ã] => &gcy;
    [ä] => &dcy;
    [å] => &iecy;
    [æ] => &zhcy;
    [ç] => &zcy;
    [è] => &icy;
    [é] => &jcy;
    [ê] => &kcy;
    [ë] => &lcy;
    [ì] => &mcy;
    [í] => &ncy;
    [î] => &ocy;
    [ï] => &pcy;
    [ð] => &rcy;
    [ñ] => &scy;
    [ò] => &tcy;
    [ó] => &ucy;
    [ô] => &fcy;
    [õ] => &khcy;
    [ö] => &tscy;
    [÷] => &chcy;
    [ø] => &shcy;
    [ù] => &shchcy;
    [ú] => &hardcy;
    [û] => &ycy;
    [ü] => &softcy;
    [ý] => &ecy;
    [þ] => &yucy;
    [ÿ] => &yacy;
)
-- with table = HTML_ENTITIES, ENT_QUOTES --
int(158)
-- with table = HTML_ENTITIES, ENT_NOQUOTES --
int(156)
-- with table = HTML_SPECIALCHARS, ENT_COMPAT --
int(4)
Array
(
    ["] => &quot;
    [&] => &amp;
    [<] => &lt;
    [>] => &gt;
)
-- with table = HTML_SPECIALCHARS, ENT_QUOTES --
array(5) {
  ["""]=>
  string(6) "&quot;"
  ["&"]=>
  string(5) "&amp;"
  ["'"]=>
  string(6) "&apos;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
}
-- with table = HTML_SPECIALCHARS, ENT_NOQUOTES --
array(3) {
  ["&"]=>
  string(5) "&amp;"
  ["<"]=>
  string(4) "&lt;"
  [">"]=>
  string(4) "&gt;"
}
Done
