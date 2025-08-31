--TEST--
GH-15268 (heap buffer overflow in phpdbg (zend_hash_num_elements() Zend/zend_hash.h))
--SKIPIF--
<?php
if (function_exists('opcache_get_status')) die('skip not for opcache because it will link');
?>
--FILE--
<?php
class B extends A {
}
class A {
}
?>
--PHPDBG--
i classes
q
--EXPECTF--
[Successful compilation of %s]
prompt> [User Classes (2)]
User Class B (0)
|-------- User Class A (not yet linked because declaration for parent was not encountered when declaring the class)
|---- in %s on line %d
User Class A (0)
|---- in %s on line %d
prompt>
