--TEST--
DBA Split Test
--SKIPIF--
<?php
    require_once __DIR__ .'/skipif.inc';
    die("info $HND handler used");
?>
--FILE--
<?php
var_dump(dba_key_split(null));
var_dump(dba_key_split(false));
var_dump(dba_key_split(1));
var_dump(dba_key_split(""));
var_dump(dba_key_split("name1"));
var_dump(dba_key_split("[key1"));
var_dump(dba_key_split("[key1]"));
var_dump(dba_key_split("key1]"));
var_dump(dba_key_split("[key1]name1"));
var_dump(dba_key_split("[key1]name1[key2]name2"));
var_dump(dba_key_split("[key1]name1"));

?>
--EXPECT--
bool(false)
bool(false)
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(1) "1"
}
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
