--TEST--
ZendMM Observer: Observe garbage collection
--EXTENSIONS--
zend_test
--INI--
zend_test.zend_mm_observer.enabled=0
opcache.enable=0
--FILE--
<?php
ini_set('zend_test.zend_mm_observer.enabled', 'true');
gc_mem_caches();
ini_set('zend_test.zend_mm_observer.enabled', 'false');
?>
--EXPECTREGEX--
.*garbage collection ended with \d+ bytes collected.*
