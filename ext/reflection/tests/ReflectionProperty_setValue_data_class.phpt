--TEST--
Test ReflectionProperty::setValue() on data class
--FILE--
<?php

data class Box {
    public function __construct(
        public $value,
    ) {}
}

function getBoxByValue() {
    return new Box(1);
}

$returnByReference = new Box(1);
function &getBoxByReference() {
    global $returnByReference;
    return $returnByReference;
}

const BOX = new Box(1);

class Prop {
    public function __construct(
        public $box,
    ) {}
}

class ReadonlyProp {
    public function __construct(
        public readonly Box $box,
    ) {}
}

$reflection = new ReflectionProperty(Box::class, 'value');

echo "CV\n";
$box = new Box(1);
$copy = $box;
$reflection->setValue($box, 2);
var_dump($box->value);
var_dump($copy->value);

echo "\nReturn by value\n";
try {
    $reflection->setValue(getBoxByValue(), 2);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

echo "\nReturn by reference\n";
$copy = getBoxByReference();
$reflection->setValue(getBoxByReference(), 2);
var_dump(getBoxByReference()->value);
var_dump($copy->value);

echo "\nConst\n";
try {
    $reflection->setValue(BOX, 2);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

echo "\nProp\n";
$prop = new Prop(new Box(1));
$copy = $prop->box;
$reflection->setValue($prop->box, 2);
var_dump($prop->box->value);
var_dump($copy->value);

echo "\nReadonly prop\n";
$readonlyProp = new ReadonlyProp(new Box(1));
try {
    $reflection->setValue($readonlyProp->box, 2);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
CV
int(2)
int(1)

Return by value
Instance of data class Box must be passed by reference

Return by reference
int(2)
int(1)

Const
Instance of data class Box must be passed by reference

Prop
int(2)
int(1)

Readonly prop
Cannot modify readonly property ReadonlyProp::$box
