--TEST--
ZendMM Observer: Observe lifecycle (RINIT -> RSHUTDOWN)
--EXTENSIONS--
zend_test
--INI--
zend_test.zend_mm_observer.enabled=1
opcache.enable=0
--FILE--
<?php
echo "done";
?>
--EXPECTREGEX--
.*ZendMM Observer enabled.*
