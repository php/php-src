--TEST--
is_zend_ptr() is broken for huge blocks
--FILE--
<?php

// Ensure there is at least one huge_block
$str = str_repeat('a', 2*1024*1024);

// Check that is_zend_ptr() does not crash
zend_test_is_zend_ptr(0);
zend_test_is_zend_ptr(1<<30);

?>
==DONE==
--EXPECT--
==DONE==
