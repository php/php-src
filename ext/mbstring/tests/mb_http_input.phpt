--TEST--
mb_http_input()
--EXTENSIONS--
mbstring
--POST--
a=���ܸ�0123456789���ܸ쥫�����ʤҤ餬��
--GET--
b=���ܸ�0123456789���ܸ쥫�����ʤҤ餬��
--INI--
mbstring.encoding_translation=1
input_encoding=latin1
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
try {
    var_dump(mb_http_input('Q'));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
ÆüËÜ¸ì0123456789ÆüËÜ¸ì¥«¥¿¥«¥Ê¤Ò¤é¤¬¤Ê
ÆüËÜ¸ì0123456789ÆüËÜ¸ì¥«¥¿¥«¥Ê¤Ò¤é¤¬¤Ê
string(10) "ISO-8859-1"
string(10) "ISO-8859-1"
bool(false)
bool(false)
array(1) {
  [0]=>
  string(10) "ISO-8859-1"
}
string(10) "ISO-8859-1"
mb_http_input(): Argument #1 ($type) must be one of "G", "P", "C", "S", "I", or "L"
