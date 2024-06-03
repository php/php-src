--TEST--
mb_http_input() with pass encoding
--EXTENSIONS--
mbstring
--POST--
a=日本語0123456789日本語カタカナひらがな
--GET--
b=%C6%FC%CB%DC%B8%EC0123456789%C6%FC%CB%DC%B8%EC%A5%AB%A5%BF%A5%AB%A5%CA%A4%D2%A4%E9%A4%AC%A4%CA
--INI--
mbstring.encoding_translation=1
input_encoding=pass
--FILE--
<?php

echo $_POST['a']."\n";
echo $_GET['b']."\n";

// Get encoding
var_dump(mb_http_input('P'));
var_dump(mb_http_input('G'));
var_dump(mb_http_input('C'));
var_dump(mb_http_input('S'));
var_dump(mb_http_input('I'));
var_dump(mb_http_input('L'));

?>
--EXPECT--
日本語0123456789日本語カタカナひらがな
日本語0123456789日本語カタカナひらがな
string(4) "pass"
string(4) "pass"
bool(false)
bool(false)
array(1) {
  [0]=>
  string(4) "pass"
}
string(4) "pass"
