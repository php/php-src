--TEST--
GMP serialization and unserialization
--EXTENSIONS--
gmp
--FILE--
<?php

var_dump($n = gmp_init(42));
var_dump($s = serialize($n));
var_dump(unserialize($s));

$n = gmp_init(13);
$n->foo = "bar";
var_dump($s = serialize($n));
var_dump(unserialize($s));

var_dump(unserialize('C:3:"GMP":15:{s:2:"42";a:0:{}}'));

try {
    unserialize('C:3:"GMP":0:{}');
} catch (Exception $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }

try {
    unserialize('C:3:"GMP":9:{s:2:"42";}');
} catch (Exception $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }

try {
    unserialize('O:3:"GMP":0:{}');
} catch (Exception $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }

try {
    unserialize('O:3:"GMP":1:{i:0;i:0;}');
} catch (Exception $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }

try {
    unserialize('O:3:"GMP":1:{i:0;s:0:"";}');
} catch (Exception $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }

try {
    unserialize('O:3:"GMP":2:{i:0;s:1:"0";i:1;i:0;}');
} catch (Exception $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }

?>
--EXPECTF--
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "42"
}
string(27) "O:3:"GMP":1:{i:0;s:2:"2a";}"
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "42"
}

Deprecated: Creation of dynamic property GMP::$foo is deprecated in %s on line %d
string(56) "O:3:"GMP":2:{i:0;s:1:"d";i:1;a:1:{s:3:"foo";s:3:"bar";}}"

Deprecated: Creation of dynamic property GMP::$foo is deprecated in %s on line %d
object(GMP)#%d (2) {
  ["foo"]=>
  string(3) "bar"
  ["num"]=>
  string(2) "13"
}
object(GMP)#1 (1) {
  ["num"]=>
  string(2) "42"
}
Exception: Could not unserialize number
Exception: Could not unserialize properties
Exception: Could not unserialize number
Exception: Could not unserialize number
Exception: Could not unserialize number
Exception: Could not unserialize properties
