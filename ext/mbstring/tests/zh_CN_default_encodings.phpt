--TEST--
Default encodings in Simplified Chinese
--EXTENSIONS--
mbstring
--INI--
mbstring.language=Simplified Chinese
--FILE--
<?php
var_dump(mb_detect_order());

?>
--EXPECT--
array(5) {
  [0]=>
  string(5) "ASCII"
  [1]=>
  string(5) "UTF-8"
  [2]=>
  string(6) "EUC-CN"
  [3]=>
  string(5) "CP936"
  [4]=>
  string(12) "GB18030-2022"
}
