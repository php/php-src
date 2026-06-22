--TEST--
Test internal class using multiple traits via zend_class_use_internal_traits
--EXTENSIONS--
zend_test
--FILE--
<?php

// Constants from both traits
var_dump(_ZendTestClassWithMultipleTraits::ZEND_TRAIT_CONST);
var_dump(_ZendTestClassWithMultipleTraits::ZEND_TRAIT_CONST2);

$obj = new _ZendTestClassWithMultipleTraits();

// Property from trait 1
var_dump($obj->traitProp);

// Methods from both traits
var_dump($obj->traitMethod());
var_dump($obj->traitMethod2());

// Static property from trait 2
var_dump(_ZendTestClassWithMultipleTraits::$staticTraitProp);

// Reflection should show both traits
$rc = new ReflectionClass(_ZendTestClassWithMultipleTraits::class);
$traits = $rc->getTraitNames();
sort($traits);
var_dump(count($traits));
var_dump($traits);

echo "Done\n";
?>
--EXPECT--
int(123)
int(321)
int(456)
int(789)
int(101)
int(999)
int(2)
array(2) {
  [0]=>
  string(30) "_ZendTestTraitForInternalClass"
  [1]=>
  string(31) "_ZendTestTraitForInternalClass2"
}
Done
