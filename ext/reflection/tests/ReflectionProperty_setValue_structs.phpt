--TEST--
Test ReflectionProperty::setValue() on structs
--FILE--
<?php

struct Box {
    public function __construct(
        public $value,
    ) {}

    public mutating function setNull() {
        $this->value = null;
    }
}

$box = new Box(1);

$reflection = new ReflectionProperty(Box::class, 'value');
try {
    $reflection->setValue($box, 2);
} catch (Exception $ex) {
    echo get_class($ex) . ': ' . $ex->getMessage(), "\n";
}

$reflection = new ReflectionMethod(Box::class, 'setNull');
try {
    $reflection->invoke($box);
} catch (Exception $ex) {
    echo get_class($ex) . ': ' . $ex->getMessage(), "\n";
}

?>
--EXPECT--
ReflectionException: May not set property value of struct "Box" through reflection
ReflectionException: May not invoke mutating method "Box::setNull()" through reflection
