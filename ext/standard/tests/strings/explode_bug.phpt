--TEST--
Explode/memnstr bug
--INI--
error_reporting=2047
memory_limit=256M
--FILE--
<?php
$res = explode(str_repeat("A",145999999),1);
var_dump($res);
?>
--EXPECT--
array(1) {
  [0]=>
  string(1) "1"
}
