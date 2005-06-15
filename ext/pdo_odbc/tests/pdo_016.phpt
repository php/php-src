--TEST--
PDO_ODBC: PDO_FETCH_BOUND
--SKIPIF--
<?php # vim:ft=php
require_once('skipif.inc');
?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

$DB->setAttribute(PDO_ATTR_CASE, PDO_CASE_LOWER);

require_once($PDO_TESTS . 'pdo_016.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
===INIT===
int(1)
int(1)
int(1)
string(1) "3"
array(3) {
  [0]=>
  string(7) "String0"
  [1]=>
  string(7) "String1"
  [2]=>
  string(7) "String2"
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
===ALONE===
array(1) {
  [0]=>
  string(7) "String0"
}
bool(true)
bool(true)
bool(true)
bool(true)
array(1) {
  [0]=>
  string(7) "String0"
}
array(1) {
  [1]=>
  string(7) "String1"
}
bool(true)
bool(true)
bool(true)
bool(true)
array(1) {
  [1]=>
  string(7) "String1"
}
array(1) {
  [2]=>
  string(7) "String2"
}
bool(true)
bool(true)
bool(true)
bool(true)
array(1) {
  [2]=>
  string(7) "String2"
}
===REBIND/SAME===
array(1) {
  [0]=>
  string(7) "String0"
}
bool(true)
bool(true)
string(7) "String0"
bool(true)
bool(true)
string(1) "0"
array(1) {
  [1]=>
  string(7) "String1"
}
bool(true)
bool(true)
string(7) "String1"
bool(true)
bool(true)
string(1) "1"
array(1) {
  [2]=>
  string(7) "String2"
}
bool(true)
bool(true)
string(7) "String2"
bool(true)
bool(true)
string(1) "2"
===REBIND/CONFLICT===
string(7) "String0"
string(7) "String1"
string(7) "String2"
===DONE===
