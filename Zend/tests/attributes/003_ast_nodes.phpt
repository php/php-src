--TEST--
Attributes can deal with AST nodes.
--FILE--
<?php

define('V1', strtoupper(php_sapi_name()));

#[A1([V1 => V1])]
class C1
{
    public const BAR = 'bar';
}

$ref = new \ReflectionClass(C1::class);
$attr = $ref->getAttributes();
var_dump(count($attr));

$args = $attr[0]->getArguments();
var_dump(count($args), $args[0][V1] === V1);

echo "\n";

#[A1(V1, 1 + 2, C1::class)]
class C2 { }

$ref = new \ReflectionClass(C2::class);
$attr = $ref->getAttributes();
var_dump(count($attr));

$args = $attr[0]->getArguments();
var_dump(count($args));
var_dump($args[0] === V1);
var_dump($args[1] === 3);
var_dump($args[2] === C1::class);

echo "\n";

#[A1(self::FOO, C1::BAR)]
class C3
{
    private const FOO = 'foo';
}

$ref = new \ReflectionClass(C3::class);
$attr = $ref->getAttributes();
var_dump(count($attr));

$args = $attr[0]->getArguments();
var_dump(count($args));
var_dump($args[0] === 'foo');
var_dump($args[1] === C1::BAR);

echo "\n";

#[ExampleWithShift(4 >> 1)]
class C4 {}
$ref = new \ReflectionClass(C4::class);
var_dump($ref->getAttributes()[0]->getArguments());

echo "\n";

#[Attribute]
class C5
{
    public function __construct() { }
}

$ref = new \ReflectionFunction(#[C5(MissingClass::SOME_CONST)] function () { });
$attr = $ref->getAttributes();
var_dump(count($attr));

try {
    $attr[0]->getArguments();
} catch (\Error $e) {
    var_dump($e->getMessage());
}

try {
    $attr[0]->newInstance();
} catch (\Error $e) {
    var_dump($e->getMessage());
}

?>
--EXPECT--
int(1)
int(1)
bool(true)

int(1)
int(3)
bool(true)
bool(true)
bool(true)

int(1)
int(2)
bool(true)
bool(true)

array(1) {
  [0]=>
  int(2)
}

int(1)
string(30) "Class "MissingClass" not found"
string(30) "Class "MissingClass" not found"
