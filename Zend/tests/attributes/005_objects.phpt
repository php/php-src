--TEST--
Attributes can be converted into objects.
--FILE--
<?php

#[Attribute(Attribute::TARGET_FUNCTION)]
class A1
{
    public string $name;
    public int $ttl;

    public function __construct(string $name, int $ttl = 50)
    {
        $this->name = $name;
        $this->ttl = $ttl;
    }
}

$ref = new \ReflectionFunction(#[A1('test')] function () { });

foreach ($ref->getAttributes() as $attr) {
    $obj = $attr->newInstance();

    var_dump(get_class($obj), $obj->name, $obj->ttl);
}

echo "\n";

$ref = new \ReflectionFunction(#[A1] function () { });

try {
    $ref->getAttributes()[0]->newInstance();
} catch (\ArgumentCountError $e) {
    var_dump('ERROR 1', $e->getMessage());
}

echo "\n";

$ref = new \ReflectionFunction(#[A1([])] function () { });

try {
    $ref->getAttributes()[0]->newInstance();
} catch (\TypeError $e) {
    var_dump('ERROR 2', $e->getMessage());
}

echo "\n";

$ref = new \ReflectionFunction(#[A2] function () { });

try {
    $ref->getAttributes()[0]->newInstance();
} catch (\Error $e) {
    var_dump('ERROR 3', $e->getMessage());
}

echo "\n";

#[Attribute]
class A3
{
    private function __construct() { }
}

$ref = new \ReflectionFunction(#[A3] function () { });

try {
    $ref->getAttributes()[0]->newInstance();
} catch (\Error $e) {
    var_dump('ERROR 4', $e->getMessage());
}

echo "\n";

#[Attribute]
class A4 { }

$ref = new \ReflectionFunction(#[A4(1)] function () { });

try {
    $ref->getAttributes()[0]->newInstance();
} catch (\Error $e) {
    var_dump('ERROR 5', $e->getMessage());
}

echo "\n";

class A5 { }

$ref = new \ReflectionFunction(#[A5] function () { });

try {
    $ref->getAttributes()[0]->newInstance();
} catch (\Error $e) {
    var_dump('ERROR 6', $e->getMessage());
}

?>
--EXPECTF--
string(2) "A1"
string(4) "test"
int(50)

string(7) "ERROR 1"
string(%d) "Too few arguments to function A1::__construct(), 0 passed in %s005_objects.php on line 26 and at least 1 expected"

string(7) "ERROR 2"
string(%d) "A1::__construct(): Argument #1 ($name) must be of type string, array given, called in %s005_objects.php on line 36"

string(7) "ERROR 3"
string(30) "Attribute class "A2" not found"

string(7) "ERROR 4"
string(48) "Attribute constructor of class A3 must be public"

string(7) "ERROR 5"
string(69) "Attribute class A4 does not have a constructor, cannot pass arguments"

string(7) "ERROR 6"
string(55) "Attempting to use non-attribute class "A5" as attribute"
