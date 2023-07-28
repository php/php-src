--TEST--
ZendMM Observer: Observe all
--EXTENSIONS--
zend_test
--INI--
zend_test.zend_mm_observer.enabled=0
opcache.enable=0
--FILE--
<?php
ini_set('zend_test.zend_mm_observer.enabled', 'true');
$str0 = str_repeat("ZendMM Observer", 100);
ini_set('zend_test.zend_mm_observer.enabled', 'false');

$str1 = str_repeat("ZendMM Observer", 100);

ini_set('zend_test.zend_mm_observer.enabled', 'true');
$str2 = str_repeat("ZendMM Observer", 100);
unset($str2);
ini_set('zend_test.zend_mm_observer.enabled', 'false');
?>
--EXPECTF--
malloc 0x%s of size %d (block: %d)
malloc 0x%s of size %d (block: %d)
freed 0x%s of size %d
