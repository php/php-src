--TEST--
zend_class_use_internal_traits with num_traits=0 (no-op behavior)
--EXTENSIONS--
zend_test
--FILE--
<?php

// Calling with zero traits should be a no-op (returns without error in release builds)
zend_test_use_internal_traits_zero();

// Verify the driver class has no traits
$rc = new ReflectionClass('_ZendTestInternalTraitsDriver');
var_dump(count($rc->getTraitNames()));

echo "Done\n";
?>
--EXPECT--
int(0)
Done
