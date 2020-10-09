--TEST--
Test basic argv multibyte API integration
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');

$argv_fl = __DIR__ . DIRECTORY_SEPARATOR . "argv_test.php";
file_put_contents($argv_fl, "<?php var_dump(\$argv); ?>");

var_dump(`$php -n $argv_fl 多字节字符串 マルチバイト文字列 многобайтоваястрока flerbytesträng`);

@unlink($argv_fl);

?>
--EXPECTF--
string(%d) "array(%d) {
  [0]=>
  string(%d) "%sargv_test.php"
  [1]=>
  string(18) "多字节字符串"
  [2]=>
  string(27) "マルチバイト文字列"
  [3]=>
  string(38) "многобайтоваястрока"
  [4]=>
  string(15) "flerbytesträng"
}
"
