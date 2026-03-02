--TEST--
Disjunctive Normal Form types in reflection
--FILE--
<?php

function dumpType(ReflectionType $rt, int $indent = 0) {
    $str_indent = str_repeat(' ', 2 * $indent);
    echo $str_indent . "Type $rt is " . $rt::class . ":\n";
    echo $str_indent . "Allows Null: " . json_encode($rt->allowsNull()) . "\n";
    foreach ($rt->getTypes() as $type) {
        if ($type instanceof ReflectionNamedType) {
            echo $str_indent . "  Name: " . $type->getName() . "\n";
            echo $str_indent . "  String: " . (string) $type . "\n";
        } else {
            dumpType($type, $indent+1);
        }
    }
}

function test1(): (X&Y)|(Z&Traversable)|null { }

class Test {
    public (X&Y)|null $prop;
}

dumpType((new ReflectionFunction('test1'))->getReturnType());

$rc = new ReflectionClass(Test::class);
$rp = $rc->getProperty('prop');
dumpType($rp->getType());

/* Force CE resolution of the property type */

interface y {}
class x implements Y, Countable {
    public function count(): int { return 0; }
}
$test = new Test;
$test->prop = new x;

$rp = $rc->getProperty('prop');
dumpType($rp->getType());

?>
--EXPECT--
Type (X&Y)|(Z&Traversable)|null is ReflectionUnionType:
Allows Null: true
  Type X&Y is ReflectionIntersectionType:
  Allows Null: false
    Name: X
    String: X
    Name: Y
    String: Y
  Type Z&Traversable is ReflectionIntersectionType:
  Allows Null: false
    Name: Z
    String: Z
    Name: Traversable
    String: Traversable
  Name: null
  String: null
Type (X&Y)|null is ReflectionUnionType:
Allows Null: true
  Type X&Y is ReflectionIntersectionType:
  Allows Null: false
    Name: X
    String: X
    Name: Y
    String: Y
  Name: null
  String: null
Type (X&Y)|null is ReflectionUnionType:
Allows Null: true
  Type X&Y is ReflectionIntersectionType:
  Allows Null: false
    Name: X
    String: X
    Name: Y
    String: Y
  Name: null
  String: null
