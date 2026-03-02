--TEST--
mb_http_input() with pass encoding
--EXTENSIONS--
mbstring
--POST--
a=日本語0123456789日本語カタカナひらがな
--GET--
b=日本語0123456789日本語カタカナひらがな
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
