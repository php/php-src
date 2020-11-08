--TEST--
ldap_read() does not modify $attributes array
--SKIPIF--
<?php
if (!extension_loaded('ldap')) die('skip ldap extension not available');
?>
--FILE--
<?php
$array = [123, 456, 789];
try {
  ldap_read(null, null, null, $array);
} catch (TypeError $err) {}
var_dump($array);
?>
--EXPECT--
array(3) {
  [0]=>
  int(123)
  [1]=>
  int(456)
  [2]=>
  int(789)
}
