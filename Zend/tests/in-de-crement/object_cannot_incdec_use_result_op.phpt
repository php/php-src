--TEST--
Cannot increment/decrement objects
--FILE--
<?php
class Foo { }
$o = new Foo;

try {
    $y = $o++;
} catch (\TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
    var_dump($o);
}
try {
    $y = $o--;
} catch (\TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
    var_dump($o);
}
try {
    $y = ++$o;
} catch (\TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
    var_dump($o);
}
try {
    $y = --$o;
} catch (\TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
    var_dump($o);
}
?>
--EXPECT--
Cannot increment Foo
object(Foo)#1 (0) {
}
Cannot decrement Foo
object(Foo)#1 (0) {
}
Cannot increment Foo
object(Foo)#1 (0) {
}
Cannot decrement Foo
object(Foo)#1 (0) {
}
