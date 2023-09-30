--TEST--
mb_trim() function tests
--EXTENSIONS--
mbstring
--FILE--
<?php
mb_internal_encoding("UTF-8");

echo "== Copy from trim ==\n";
var_dump('ABC' ===  mb_trim('ABC'));
var_dump('ABC' === mb_ltrim('ABC'));
var_dump('ABC' === mb_rtrim('ABC'));
var_dump('ABC' ===  mb_trim(" \0\t\nABC \0\t\n"));
var_dump("ABC \0\t\n" === mb_ltrim(" \0\t\nABC \0\t\n"));
var_dump(" \0\t\nABC" === mb_rtrim(" \0\t\nABC \0\t\n"));
var_dump(" \0\t\nABC \0\t\n" ===  mb_trim(" \0\t\nABC \0\t\n",''));
var_dump(" \0\t\nABC \0\t\n" === mb_ltrim(" \0\t\nABC \0\t\n",''));
var_dump(" \0\t\nABC \0\t\n" === mb_rtrim(" \0\t\nABC \0\t\n",''));
echo "== Empty string ==\n";
var_dump(mb_trim(""));
var_dump(mb_ltrim(""));
var_dump(mb_rtrim(""));

echo "== Single string ==\n";
var_dump(mb_ltrim(' test ', ''));
var_dump(mb_trim("　あいうえおあお　", "　", "UTF-8"));
var_dump(mb_trim('foo BAR Spaß', 'ß', "UTF-8"));
var_dump(mb_trim('foo BAR Spaß', 'f', "UTF-8"));

echo "== Multi strings ==\n";
var_dump(mb_trim('foo BAR Spaß', 'ßf', "UTF-8"));
var_dump(mb_trim('foo BAR Spaß', 'fß', "UTF-8"));
var_dump(mb_trim("　あいうおえお 　あ", "　あ", "UTF-8"));
var_dump(mb_trim("　あいうおえお 　あ", "あ　", "UTF-8"));
var_dump(mb_trim("　あいうおえお 　a", "あa", "UTF-8"));
var_dump(mb_trim("　あいうおえお 　a", "\xe3", "UTF-8"));

echo "== Many strings ==\n";
var_dump(mb_trim(str_repeat("　", 129)));
var_dump(mb_trim(str_repeat("　", 129) . "a"));
var_dump(mb_rtrim(str_repeat("　", 129) . "a"));

echo "== mb_ltrim ==\n";
var_dump(mb_ltrim("あああああああああああああああああああああああああああああああああいああああ", "あ"));
echo "== mb_rtrim ==\n";
var_dump(mb_rtrim("あああああああああああああああああああああああああああああああああいああああ", "あ"));

echo "== default params ==\n";
var_dump(mb_trim(" \f\n\r\v\x00\u{00A0}\u{1680}\u{2000}\u{2001}\u{2002}\u{2003}\u{2004}\u{2005}\u{2006}\u{2007}\u{2008}\u{2009}\u{200A}\u{2028}\u{2029}\u{202F}\u{205F}\u{3000}\u{0085}\u{180E}"));

echo "== Byte Order Mark ==\n";
var_dump(mb_ltrim("\u{FFFE}漢字", "\u{FFFE}\u{FEFF}"));
var_dump(bin2hex(mb_ltrim(mb_convert_encoding("\u{FFFE}漢字", "UTF-16LE", "UTF-8"), mb_convert_encoding("\u{FFFE}\u{FEFF}", "UTF-16LE", "UTF-8"), "UTF-16LE")));
var_dump(bin2hex(mb_ltrim(mb_convert_encoding("\u{FEFF}漢字", "UTF-16BE", "UTF-8"), mb_convert_encoding("\u{FFFE}\u{FEFF}", "UTF-16BE", "UTF-8"), "UTF-16BE")));

echo "== Empty string ==\n";
var_dump(mb_trim(" abcd ", ""));
var_dump(mb_ltrim(" abcd ", ""));
var_dump(mb_rtrim(" abcd ", ""));

echo "== SJIS ==\n";
var_dump(mb_convert_encoding(mb_trim("\x81\x40\x82\xa0\x81\x40", "\x81\x40", "SJIS"), "UTF-8", "SJIS"));

echo "== Same strings ==\n";
var_dump(mb_trim("foo", "oo"));

echo "== \$encoding throws ValueError ==\n";
try {
    var_dump(mb_trim( "\u{180F}", "", "NULL"));
} catch (ValueError $e) {
    var_dump($e->getMessage());
}

?>
--EXPECT--
== Copy from trim ==
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
== Empty string ==
string(0) ""
string(0) ""
string(0) ""
== Single string ==
string(6) " test "
string(21) "あいうえおあお"
string(11) "foo BAR Spa"
string(12) "oo BAR Spaß"
== Multi strings ==
string(10) "oo BAR Spa"
string(10) "oo BAR Spa"
string(16) "いうおえお "
string(16) "いうおえお "
string(25) "　あいうおえお 　"
string(26) "　あいうおえお 　a"
== Many strings ==
string(0) ""
string(1) "a"
string(388) "　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　a"
== mb_ltrim ==
string(15) "いああああ"
== mb_rtrim ==
string(102) "あああああああああああああああああああああああああああああああああい"
== default params ==
string(0) ""
== Byte Order Mark ==
string(6) "漢字"
string(8) "226f575b"
string(8) "6f225b57"
== Empty string ==
string(6) " abcd "
string(6) " abcd "
string(6) " abcd "
== SJIS ==
string(3) "あ"
== Same strings ==
string(1) "f"
== $encoding throws ValueError ==
string(73) "mb_trim(): Argument #3 ($encoding) must be a valid encoding, "NULL" given"
