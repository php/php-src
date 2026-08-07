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
    echo 'ERROR 1: ', $e::class, ': ', $e->getMessage(), "\n";
}

echo "\n";

$ref = new \ReflectionFunction(#[A1([])] function () { });

try {
    $ref->getAttributes()[0]->newInstance();
} catch (\TypeError $e) {
    echo 'ERROR 2: ', $e::class, ': ', $e->getMessage(), "\n";
}

echo "\n";

$ref = new \ReflectionFunction(#[A2] function () { });

try {
    $ref->getAttributes()[0]->newInstance();
} catch (\Error $e) {
    echo 'ERROR 3: ', $e::class, ': ', $e->getMessage(), "\n";
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
    echo 'ERROR 4: ', $e::class, ': ', $e->getMessage(), "\n";
}

echo "\n";

class A5 { }

$ref = new \ReflectionFunction(#[A5] function () { });

try {
    $ref->getAttributes()[0]->newInstance();
} catch (\Error $e) {
    echo 'ERROR 6: ', $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
string(2) "A1"
string(4) "test"
int(50)

ERROR 1: ArgumentCountError: Too few arguments to function A1::__construct(), 0 passed in %s005_objects.php on line 26 and at least 1 expected

ERROR 2: TypeError: A1::__construct(): Argument #1 ($name) must be of type string, array given, called in %s005_objects.php on line 36

ERROR 3: Error: Attribute class "A2" not found

ERROR 4: Error: Call to private A3::__construct() from global scope

ERROR 6: Error: Attempting to use non-attribute class "A5" as attribute
