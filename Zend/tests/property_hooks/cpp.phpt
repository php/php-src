--TEST--
Constructor property promotion
--FILE--
<?php

class Test {
    public function __construct(
        public $prop = 42 {
            get => print("Getting\n");
            set { print("Setting\n"); }
        }
    ) {
        echo "Constructor\n";
    }
}

echo "Pre-test\n";
$test = new Test;
$test->prop;
$test->prop = 42;

$r = (new ReflectionProperty(Test::class, 'prop'));
var_dump($r->hasDefaultValue());
var_dump($r->getDefaultValue());

?>
--EXPECTF--
Pre-test
Setting
Constructor
Getting
Setting
bool(false)

Deprecated: ReflectionProperty::getDefaultValue() for a property without a default value is deprecated, use ReflectionProperty::hasDefaultValue() to check if the default value exists in %s on line %d
NULL
