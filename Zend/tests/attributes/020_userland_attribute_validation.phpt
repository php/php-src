--TEST--
Attributes expose and verify target and repeatable data.
--FILE--
<?php

#[Attribute(Attribute::TARGET_FUNCTION | Attribute::TARGET_METHOD)]
class A1 { }

$ref = new \ReflectionFunction(#[A1] function () { });
$attr = $ref->getAttributes()[0];
var_dump($attr->getName(), $attr->getTarget() == Attribute::TARGET_FUNCTION, $attr->isRepeated());
var_dump(get_class($attr->newInstance()));

echo "\n";

$ref = new \ReflectionObject(new #[A1] class() { });
$attr = $ref->getAttributes()[0];
var_dump($attr->getName(), $attr->getTarget() == Attribute::TARGET_CLASS, $attr->isRepeated());

try {
    $attr->newInstance();
} catch (\Throwable $e) {
    var_dump('ERROR 1', $e->getMessage());
}

echo "\n";

$ref = new \ReflectionFunction(#[A1] #[A1] function () { });
$attr = $ref->getAttributes()[0];
var_dump($attr->getName(), $attr->getTarget() == Attribute::TARGET_FUNCTION, $attr->isRepeated());

try {
    $attr->newInstance();
} catch (\Throwable $e) {
    var_dump('ERROR 2', $e->getMessage());
}

echo "\n";

#[Attribute(Attribute::TARGET_CLASS | Attribute::IS_REPEATABLE)]
class A2 { }

$ref = new \ReflectionObject(new #[A2] #[A2] class() { });
$attr = $ref->getAttributes()[0];
var_dump($attr->getName(), $attr->getTarget() == Attribute::TARGET_CLASS, $attr->isRepeated());
var_dump(get_class($attr->newInstance()));

?>
--EXPECT--
string(2) "A1"
bool(true)
bool(false)
string(2) "A1"

string(2) "A1"
bool(true)
bool(false)
string(7) "ERROR 1"
string(70) "Attribute "A1" cannot target class (allowed targets: function, method)"

string(2) "A1"
bool(true)
bool(true)
string(7) "ERROR 2"
string(35) "Attribute "A1" must not be repeated"

string(2) "A2"
bool(true)
bool(true)
string(2) "A2"
