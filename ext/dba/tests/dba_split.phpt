--TEST--
DBA Split Test
--SKIPIF--
<?php
	require_once dirname(__FILE__) .'/skipif.inc';
	die("info $HND handler used");
?>
--FILE--
<?php
var_dump(dba_key_split("key1", "name"));
var_dump(dba_key_split(1));
var_dump(dba_key_split(null));
var_dump(dba_key_split(""));
var_dump(dba_key_split("name1"));
var_dump(dba_key_split("[key1"));
var_dump(dba_key_split("[key1]"));
var_dump(dba_key_split("key1]"));
var_dump(dba_key_split("[key1]name1"));
var_dump(dba_key_split("[key1]name1[key2]name2"));
var_dump(dba_key_split("[key1]name1"));

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Warning: Wrong parameter count for dba_key_split() in %sdba_split.php on line %d
NULL
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(1) "1"
}
bool(false)
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
}
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(5) "name1"
}
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(5) "[key1"
}
array(2) {
  [0]=>
  string(4) "key1"
  [1]=>
  string(0) ""
}
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(5) "key1]"
}
array(2) {
  [0]=>
  string(4) "key1"
  [1]=>
  string(5) "name1"
}
array(2) {
  [0]=>
  string(4) "key1"
  [1]=>
  string(16) "name1[key2]name2"
}
array(2) {
  [0]=>
  string(4) "key1"
  [1]=>
  string(5) "name1"
}
===DONE===
