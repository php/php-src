--TEST--
ZendMM Observer: Observe shutdown
--EXTENSIONS--
zend_test
--INI--
zend_test.zend_mm_observer.enabled=1
--FILE--
<?php
echo "done.";
?>
--EXPECTREGEX--
.*shutdown in progress: full\(0\), silent\(1\).*
