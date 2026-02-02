--TEST--
zend_class_use_internal_traits with non-trait class produces E_COMPILE_ERROR
--EXTENSIONS--
zend_test
--FILE--
<?php

zend_test_use_internal_traits_not_trait();

echo "Should not reach here\n";
?>
--EXPECTF--
Fatal error: Class %s cannot use %s - it is not a trait in %s on line %d
