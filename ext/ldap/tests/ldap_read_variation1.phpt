--TEST--
ldap_read() does not modify $attributes array
--EXTENSIONS--
ldap
--FILE--
<?php
$array = [123, 456, 789];
try {
  ldap_read(null, '', '', $array);
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
