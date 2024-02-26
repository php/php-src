--TEST--
ZendMM Observer: Observe free
--EXTENSIONS--
zend_test
--INI--
zend_test.zend_mm_observer.enabled=0
opcache.enable=0
--FILE--
<?php
$string = str_repeat("ZendMM Observer", 100);
ini_set('zend_test.zend_mm_observer.enabled', 'true');
unset($string);
ini_set('zend_test.zend_mm_observer.enabled', 'false');
?>
--EXPECTREGEX--
.*freed \S+ of size \d+.*
