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
    [�] => &DJcy;
    [�] => &GJcy;
    [�] => &sbquo;
    [�] => &gjcy;
    [�] => &bdquo;
    [�] => &hellip;
    [�] => &dagger;
    [�] => &Dagger;
    [�] => &euro;
    [�] => &permil;
    [�] => &LJcy;
    [�] => &lsaquo;
    [�] => &NJcy;
    [�] => &KJcy;
    [�] => &TSHcy;
    [�] => &DZcy;
    [�] => &djcy;
    [�] => &OpenCurlyQuote;
    [�] => &rsquo;
    [�] => &OpenCurlyDoubleQuote;
    [�] => &rdquo;
    [�] => &bull;
    [�] => &ndash;
    [�] => &mdash;
    [�] => &trade;
    [�] => &ljcy;
    [�] => &rsaquo;
    [�] => &njcy;
    [�] => &kjcy;
    [�] => &tshcy;
    [�] => &dzcy;
    [�] => &nbsp;
    [�] => &Ubrcy;
    [�] => &ubrcy;
    [�] => &Jsercy;
    [�] => &curren;
    [�] => &brvbar;
    [�] => &sect;
    [�] => &IOcy;
    [�] => &copy;
    [�] => &Jukcy;
    [�] => &laquo;
    [�] => &not;
    [�] => &shy;
    [�] => &reg;
    [�] => &YIcy;
    [�] => &deg;
    [�] => &plusmn;
    [�] => &Iukcy;
    [�] => &iukcy;
    [�] => &micro;
    [�] => &para;
    [�] => &CenterDot;
    [�] => &iocy;
    [�] => &numero;
    [�] => &jukcy;
    [�] => &raquo;
    [�] => &jsercy;
    [�] => &DScy;
    [�] => &dscy;
    [�] => &yicy;
    [�] => &Acy;
    [�] => &Bcy;
    [�] => &Vcy;
    [�] => &Gcy;
    [�] => &Dcy;
    [�] => &IEcy;
    [�] => &ZHcy;
    [�] => &Zcy;
    [�] => &Icy;
    [�] => &Jcy;
    [�] => &Kcy;
    [�] => &Lcy;
    [�] => &Mcy;
    [�] => &Ncy;
    [�] => &Ocy;
    [�] => &Pcy;
    [�] => &Rcy;
    [�] => &Scy;
    [�] => &Tcy;
    [�] => &Ucy;
    [�] => &Fcy;
    [�] => &KHcy;
    [�] => &TScy;
    [�] => &CHcy;
    [�] => &SHcy;
    [�] => &SHCHcy;
    [�] => &HARDcy;
    [�] => &Ycy;
    [�] => &SOFTcy;
    [�] => &Ecy;
    [�] => &YUcy;
    [�] => &YAcy;
    [�] => &acy;
    [�] => &bcy;
    [�] => &vcy;
    [�] => &gcy;
    [�] => &dcy;
    [�] => &iecy;
    [�] => &zhcy;
    [�] => &zcy;
    [�] => &icy;
    [�] => &jcy;
    [�] => &kcy;
    [�] => &lcy;
    [�] => &mcy;
    [�] => &ncy;
    [�] => &ocy;
    [�] => &pcy;
    [�] => &rcy;
    [�] => &scy;
    [�] => &tcy;
    [�] => &ucy;
    [�] => &fcy;
    [�] => &khcy;
    [�] => &tscy;
    [�] => &chcy;
    [�] => &shcy;
    [�] => &shchcy;
    [�] => &hardcy;
    [�] => &ycy;
    [�] => &softcy;
    [�] => &ecy;
    [�] => &yucy;
    [�] => &yacy;
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
