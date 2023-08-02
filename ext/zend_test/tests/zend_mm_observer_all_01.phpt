--TEST--
ZendMM Observer: Observe all
--EXTENSIONS--
zend_test
--INI--
zend_test.zend_mm_observer.enabled=1
--FILE--
<?php
echo "done.";
?>
--EXPECTREGEX--
.*ZendMM Observer enabled.*
.*done\..*
.*ZendMM Observer disabled.*
