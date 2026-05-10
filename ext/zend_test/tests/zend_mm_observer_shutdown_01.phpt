--TEST--
ZendMM Observer: Observe shutdown
--EXTENSIONS--
zend_test
--ENV--
ZEND_TEST_MM_OBSERVER_SHUTDOWN_TEST=1
--INI--
zend_test.zend_mm_observer.enabled=1
--FILE--
<?php
echo "done.";
?>
--EXPECTREGEX--
.*shutdown in progress: full\(\d\), silent\(\d\).*
