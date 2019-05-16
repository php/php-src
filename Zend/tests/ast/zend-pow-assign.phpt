--TEST--
ZEND_POW_ASSIGN
--INI--
zend.assertions=1
--FILE--
<?php

assert_options(ASSERT_WARNING);
assert(false && ($a **= 2));
--EXPECTF--
Warning: assert(): assert(false && ($a **= 2)) failed in %s%ezend-pow-assign.php on line %d
