--TEST--
Union types in reflection
--INI--
error_reporting=E_ALL&~E_DEPRECATED
--FILE--
<?php

function dumpType(ReflectionUnionType $rt) {
    echo "Type $rt:\n";
    echo "Allows null: " . ($rt->allowsNull() ? "true" : "false") . "\n";
    foreach ($rt->getTypes() as $type) {
        echo "  Name: " . $type->getName() . "\n";
        echo "  String: " . (string) $type . "\n";
        echo "  Allows Null: " . ($type->allowsNull() ? "true" : "false") . "\n";
    }
}

function test1(): X|Y|int|float|false|null { }
function test2(): X|iterable|bool { }

class Test {
    public X|Y|int $prop;
}

dumpType((new ReflectionFunction('test1'))->getReturnType());
dumpType((new ReflectionFunction('test2'))->getReturnType());

$rc = new ReflectionClass(Test::class);
$rp = $rc->getProperty('prop');
dumpType($rp->getType());

/* Force CE resolution of the property type */

class x {}
$test = new Test;
$test->prop = new x;

$rp = $rc->getProperty('prop');
dumpType($rp->getType());

class y {}
$test->prop = new y;

$rp = $rc->getProperty('prop');
dumpType($rp->getType());

?>
--EXPECT--
Type X|Y|int|float|false|null:
Allows null: true
  Name: X
  String: X
  Allows Null: false
  Name: Y
  String: Y
  Allows Null: false
  Name: int
  String: int
  Allows Null: false
  Name: float
  String: float
  Allows Null: false
  Name: false
  String: false
  Allows Null: false
  Name: null
  String: null
  Allows Null: true
Type X|iterable|bool:
Allows null: false
  Name: X
  String: X
  Allows Null: false
  Name: iterable
  String: iterable
  Allows Null: false
  Name: bool
  String: bool
  Allows Null: false
Type X|Y|int:
Allows null: false
  Name: X
  String: X
  Allows Null: false
  Name: Y
  String: Y
  Allows Null: false
  Name: int
  String: int
  Allows Null: false
Type x|Y|int:
Allows null: false
  Name: x
  String: x
  Allows Null: false
  Name: Y
  String: Y
  Allows Null: false
  Name: int
  String: int
  Allows Null: false
Type x|y|int:
Allows null: false
  Name: x
  String: x
  Allows Null: false
  Name: y
  String: y
  Allows Null: false
  Name: int
  String: int
  Allows Null: false
