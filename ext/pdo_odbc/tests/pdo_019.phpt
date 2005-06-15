--TEST--
PDO_ODBC: fetch() and while()
--SKIPIF--
<?php # vim:ft=php
require_once('skipif.inc');
?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

$DB->setAttribute(PDO_ATTR_CASE, PDO_CASE_LOWER);

require_once($PDO_TESTS . 'pdo_019.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
===INIT===
int(1)
int(1)
int(1)
int(1)
string(1) "4"
array(4) {
  [0]=>
  string(7) "String0"
  [1]=>
  string(7) "String1"
  [2]=>
  string(7) "String2"
  [3]=>
  string(7) "String3"
}
===WHILE===
array(1) {
  [0]=>
  string(7) "String0"
}
array(1) {
  [1]=>
  string(7) "String1"
}
array(1) {
  [2]=>
  string(7) "String2"
}
array(1) {
  [3]=>
  string(7) "String3"
}
===DONE===
