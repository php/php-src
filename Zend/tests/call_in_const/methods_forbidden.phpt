--TEST--
Cannot call methods in constant expressions
--FILE--
<?php
// Currently, php will evaluate all of the instance property defaults at once and cache them
// the first time a class gets instantiated, in _object_and_properties_init.
//
// Authors of future RFCs may wish to change PHP
// to allow `count()` or `generate_unique_id()` or `public $fields = new stdClass();`
// as the defaults of instance properties.
class MyClass {
    public static function example() {
        return 'value';
    }
    const X = 'MyClass::example'();
}
?>
--EXPECTF--
Fatal error: Constant expression contains invalid name for function call in %s on line 12
