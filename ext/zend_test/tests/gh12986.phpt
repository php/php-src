--TEST--
GH-12986 (HASH_FLAG_ALLOW_COW_VIOLATION is not preserved by zend_hash_real_init_ex)
--EXTENSIONS--
zend_test
--FILE--
<?php

zend_test_gh12986();

?>
--EXPECT--
OK: HASH_FLAG_ALLOW_COW_VIOLATION is preserved by zend_hash_real_init_ex
