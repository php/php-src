--TEST--
GMP serialization and unserialization
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump($n = gmp_init(42));
var_dump($s = serialize($n));
var_dump(unserialize($s));

$n = gmp_init(13);
$n->foo = "bar";
var_dump(unserialize(serialize($n)));

try {
    unserialize('C:3:"GMP":0:{}');
} catch (Exception $e) { var_dump($e->getMessage()); }

try {
    unserialize('C:3:"GMP":8:{s:2:"42"}');
} catch (Exception $e) { var_dump($e->getMessage()); }

?>
--EXPECTF--	
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "42"
}
string(30) "C:3:"GMP":15:{s:2:"42";a:0:{}}"
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "42"
}
object(GMP)#%d (2) {
  ["foo"]=>
  string(3) "bar"
  ["num"]=>
  string(2) "13"
}
string(28) "Could not unserialize number"
string(32) "Could not unserialize properties"
