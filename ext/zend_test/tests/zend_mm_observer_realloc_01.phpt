--TEST--
ZendMM Observer: Observe realloc
--EXTENSIONS--
zend_test
--INI--
zend_test.zend_mm_observer.enabled=0
opcache.enable=0
--FILE--
<?php
$a = [];
$a[] = 'ZendMM Observer';
$a[] = 'ZendMM Observer';
$a[] = 'ZendMM Observer';
$a[] = 'ZendMM Observer';
$a[] = 'ZendMM Observer';
$a[] = 'ZendMM Observer';
$a[] = 'ZendMM Observer';
$a[] = 'ZendMM Observer';
ini_set('zend_test.zend_mm_observer.enabled', 'true');
$a[] = 'ZendMM Observer';
ini_set('zend_test.zend_mm_observer.enabled', 'false');
?>
--EXPECTREGEX--
.*realloc 0x\S+ of size \d+ \(block: \d+, former 0x\S+\)
