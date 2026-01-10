--TEST--
mb_http_input() with POST method and multiple candidate encodings
--EXTENSIONS--
mbstring
--POST--
a=日本語&b=ελληνικά
--INI--
mbstring.encoding_translation=1
input_encoding=UTF-8,SJIS,EUC-JP,ISO-8859-1
--FILE--
<?php

echo $_POST['a']."\n";
echo $_POST['b']."\n";

// Get encoding
var_dump(mb_http_input('P'));
var_dump(mb_http_input('G'));
var_dump(mb_http_input('C'));
var_dump(mb_http_input('S'));
var_dump(mb_http_input('I'));
var_dump(mb_http_input('L'));

?>
--EXPECT--
日本語
ελληνικά
string(5) "UTF-8"
bool(false)
bool(false)
bool(false)
array(4) {
  [0]=>
  string(5) "UTF-8"
  [1]=>
  string(4) "SJIS"
  [2]=>
  string(6) "EUC-JP"
  [3]=>
  string(10) "ISO-8859-1"
}
string(28) "UTF-8,SJIS,EUC-JP,ISO-8859-1"
