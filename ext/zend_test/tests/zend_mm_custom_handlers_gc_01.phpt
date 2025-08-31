--TEST--
ZendMM Custom Handlers: garbage collection
--EXTENSIONS--
zend_test
--FILE--
<?php
ini_set('zend_test.zend_mm_custom_handlers.enabled', 1);
gc_mem_caches();
ini_set('zend_test.zend_mm_custom_handlers.enabled', 0);
?>
--EXPECTREGEX--
.*ZendMM GC freed \d+ bytes.*
