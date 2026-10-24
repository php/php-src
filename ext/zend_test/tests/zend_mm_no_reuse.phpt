--TEST--
ZendMM no-reuse mode retains and clears freed storage until heap shutdown
--EXTENSIONS--
zend_test
--ENV--
USE_ZEND_ALLOC=1
ZEND_MM_NO_REUSE=1
--FILE--
<?php
foreach (zend_test_mm_no_reuse() as $check => $passed) {
    echo "$check: ", $passed ? "ok" : "failed", "\n";
}
?>
--EXPECT--
non-overlap: ok
gc: ok
realloc: ok
contents: ok
cleared: ok
shutdown: ok
