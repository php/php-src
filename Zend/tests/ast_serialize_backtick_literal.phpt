--TEST--
Serialization of backtick literal is incorrect
--INI--
zend.assertions=1
assert.exception=0
--FILE--
<?php

assert_options(ASSERT_WARNING);
assert(false && `echo -n ""`);
?>
--EXPECTF--
Warning: assert(): assert(false && `echo -n ""`) failed in %s%east_serialize_backtick_literal.php on line %d
