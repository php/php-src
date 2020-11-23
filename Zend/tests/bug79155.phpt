--TEST--
Bug #79155: Property nullability lost when using multiple property definition
--FILE--
<?php

class Foo {
    public ?string $a, $b;
    public ?stdClass $c, $d;
}

$t = new Foo;
$t->a = "str";
$t->b = "str";
$t->c = new stdClass;
$t->d = new stdClass;

var_dump($t->a, $t->b, $t->c, $t->d);

$t->a = null;
$t->b = null;
$t->c = null;
$t->d = null;
var_dump($t->a, $t->b, $t->c, $t->d);

?>
--EXPECT--
string(3) "str"
string(3) "str"
object(stdClass)#2 (0) {
}
object(stdClass)#3 (0) {
}
NULL
NULL
NULL
NULL
