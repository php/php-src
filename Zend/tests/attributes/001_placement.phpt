--TEST--
Attributes can be placed on all supported elements.
--FILE--
<?php

#[A1(1)]
class Foo
{
    #[A1(2)]
    public const FOO = 'foo';

    #[A1(3)]
    public $x;

    #[A1(4)]
    public function foo(#[A1(5)] $a, #[A1(6)] $b) { }
}

$object = new #[A1(7)] class () { };

#[A1(8)]
function f1() { }

$f2 = #[A1(9)] function () { };

$f3 = #[A1(10)] fn () => 1;

$ref = new \ReflectionClass(Foo::class);

$sources = [
    $ref,
    $ref->getReflectionConstant('FOO'),
    $ref->getProperty('x'),
    $ref->getMethod('foo'),
    $ref->getMethod('foo')->getParameters()[0],
    $ref->getMethod('foo')->getParameters()[1],
    new \ReflectionObject($object),
    new \ReflectionFunction('f1'),
    new \ReflectionFunction($f2),
    new \ReflectionFunction($f3)
];

foreach ($sources as $r) {
    $attr = $r->getAttributes();
    var_dump(get_class($r), count($attr));

    foreach ($attr as $a) {
        var_dump($a->getName(), $a->getArguments());
    }

    echo "\n";
}

?>
--EXPECT--
string(15) "ReflectionClass"
int(1)
string(2) "A1"
array(1) {
  [0]=>
  int(1)
}

string(23) "ReflectionClassConstant"
int(1)
string(2) "A1"
array(1) {
  [0]=>
  int(2)
}

string(18) "ReflectionProperty"
int(1)
string(2) "A1"
array(1) {
  [0]=>
  int(3)
}

string(16) "ReflectionMethod"
int(1)
string(2) "A1"
array(1) {
  [0]=>
  int(4)
}

string(19) "ReflectionParameter"
int(1)
string(2) "A1"
array(1) {
  [0]=>
  int(5)
}

string(19) "ReflectionParameter"
int(1)
string(2) "A1"
array(1) {
  [0]=>
  int(6)
}

string(16) "ReflectionObject"
int(1)
string(2) "A1"
array(1) {
  [0]=>
  int(7)
}

string(18) "ReflectionFunction"
int(1)
string(2) "A1"
array(1) {
  [0]=>
  int(8)
}

string(18) "ReflectionFunction"
int(1)
string(2) "A1"
array(1) {
  [0]=>
  int(9)
}

string(18) "ReflectionFunction"
int(1)
string(2) "A1"
array(1) {
  [0]=>
  int(10)
}
