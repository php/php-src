--TEST--
Test internal class using trait with static property via zend_class_use_internal_traits
--EXTENSIONS--
zend_test
--FILE--
<?php

// Default value
var_dump(_ZendTestClassWithMultipleTraits::$staticTraitProp);

// Modify static property
_ZendTestClassWithMultipleTraits::$staticTraitProp = 555;
var_dump(_ZendTestClassWithMultipleTraits::$staticTraitProp);

// Verify via reflection
$rc = new ReflectionClass(_ZendTestClassWithMultipleTraits::class);
$prop = $rc->getProperty('staticTraitProp');
var_dump($prop->isStatic());

echo "Done\n";
?>
--EXPECT--
int(999)
int(555)
bool(true)
Done
