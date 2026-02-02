--TEST--
Test internal class using trait via zend_class_use_internal_traits
--EXTENSIONS--
zend_test
--FILE--
<?php

var_dump(_ZendTestClassWithTrait::ZEND_TRAIT_CONST);

$obj = new _ZendTestClassWithTrait();
var_dump($obj->traitProp);

var_dump($obj->traitMethod());

var_dump(method_exists(_ZendTestClassWithTrait::class, 'traitMethod'));

$rc = new ReflectionClass(_ZendTestClassWithTrait::class);
$traits = $rc->getTraitNames();
var_dump(count($traits));
var_dump(in_array('_ZendTestTraitForInternalClass', $traits));

echo "Done\n";
?>
--EXPECT--
int(123)
int(456)
int(789)
bool(true)
int(1)
bool(true)
Done
