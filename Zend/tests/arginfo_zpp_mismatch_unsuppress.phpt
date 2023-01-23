--TEST--
Test don't suppress arginfo / zpp mismatch
--EXTENSIONS--
zend_test
--ENV--
ZEND_SUPPRESS_ARGINFO_ZPP_MISMATCH=0
--FILE--
<?php
zend_test_arginfo_zpp_mismatch(1);
echo 'success';
--EXPECTF--
Fatal error: Arginfo / zpp mismatch during call of zend_test_arginfo_zpp_mismatch() in %s on line %d
