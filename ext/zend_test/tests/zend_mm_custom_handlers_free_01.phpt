--TEST--
ZendMM Custom Handlers: garbage collection
--EXTENSIONS--
zend_test
--FILE--
<?php
$string = str_repeat('String', rand(1,100));
ini_set('zend_test.zend_mm_custom_handlers.enabled', 1);
unset($string);
ini_set('zend_test.zend_mm_custom_handlers.enabled', 0);
?>
--EXPECTREGEX--
.*Freed memory at.*
