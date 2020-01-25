--TEST--
Cannot call functions from defaults of instance properties
--FILE--
<?php
// Currently, php will evaluate all of the instance property defaults at once and cache them
// the first time a class gets instantiated, in _object_and_properties_init.
//
// Authors of future RFCs may wish to change PHP
// to allow `count()` or `generate_unique_id()` or `public $fields = new stdClass();`
// as the defaults of instance properties.
class MyClass {
    public $v1 = count([]);
}
?>
--EXPECTF--
Fatal error: Default value for instance property MyClass::$v1 cannot contain function calls in %s on line 9
