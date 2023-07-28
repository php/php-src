--TEST--
ZendMM Observer: Observe all
--EXTENSIONS--
zend_test
--INI--
zend_test.zend_mm_observer.enabled=1
opcache.enable=0
--FILE--
<?php
?>
--EXPECTREGEX--
.*
ZendMM Observer enabled
.*
.*
ZendMM Observer disabled
