--TEST--
GMP serialization and unserialization
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump($n = gmp_init(42));
var_dump($s = serialize($n));
var_dump(unserialize($s));

?>
--EXPECTF--	
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "42"
}
string(33) "O:3:"GMP":1:{s:3:"num";s:2:"42";}"
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "42"
}
