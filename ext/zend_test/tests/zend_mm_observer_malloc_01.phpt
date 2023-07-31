--TEST--
ZendMM Observer: Observe malloc
--EXTENSIONS--
zend_test
--INI--
zend_test.zend_mm_observer.enabled=0
opcache.enable=0
--FILE--
<?php
ini_set('zend_test.zend_mm_observer.enabled', 'true');
$string = str_repeat("ZendMM Observer", 100);
ini_set('zend_test.zend_mm_observer.enabled', 'false');
?>
--EXPECTREGEX--
.*malloc 0x\S+ of size \d+ \(block: \d+\).*
