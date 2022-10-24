--TEST--
Intersection type which is implicitly nullable should be a DNF type in reflection
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

function foo(X&Y $foo = null) {
    var_dump($foo);
}

dumpType((new ReflectionFunction('foo'))->getParameters()[0]->getType());

?>
--EXPECT--
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
