--TEST--
#[\NoDiscard]: Combining with #[\Deprecated] (Internal).
--EXTENSIONS--
zend_test
--FILE--
<?php

zend_test_deprecated_nodiscard();

?>
--EXPECTF--
Deprecated: Function zend_test_deprecated_nodiscard() is deprecated, custom message in %s on line %d

Warning: The return value of function zend_test_deprecated_nodiscard() should either be used or intentionally ignored by casting it as (void), custom message 2 in %s on line %d
