--TEST--
Literal types: a literal combined with a wide scalar type (1|string)
--FILE--
<?php
function foo(1|string $x) { return $x; }

$values = [1, 2, 0, 1.0, 2.0, 2.5, "1", "2", "foo", true, false];
foreach ($values as $v) {
    $r = foo($v);
    printf("%-7s => %s(%s)\n", var_export($v, true), gettype($r), var_export($r, true));
}

foreach ([null, [1]] as $v) {
    try {
        foo($v);
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
}
?>
--EXPECTF--
1       => integer(1)
2       => string('2')
0       => string('0')
1.0     => integer(1)
2.0     => string('2')
2.5     => string('2.5')
'1'     => string('1')
'2'     => string('2')
'foo'   => string('foo')
true    => integer(1)
false   => string('')
foo(): Argument #1 ($x) must be of type 1|string, null given, called in %s on line %d
foo(): Argument #1 ($x) must be of type 1|string, array given, called in %s on line %d
