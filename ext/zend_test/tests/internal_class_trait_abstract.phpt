--TEST--
zend_class_use_internal_traits with abstract trait method produces E_COMPILE_ERROR
--EXTENSIONS--
zend_test
--FILE--
<?php

// This should produce a fatal error because the trait has an abstract method
zend_test_use_internal_traits_abstract_trait();

echo "Should not reach here\n";
?>
--EXPECTF--
Fatal error: Internal trait binding does not support abstract trait methods in %s in %s on line %d
