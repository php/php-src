--TEST--
Cannot increment/decrement objects
--FILE--
<?php
class Foo { }
$o = new Foo;

try {
    $o++;
} catch (\TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
    var_dump($o);
}
try {
    $o--;
} catch (\TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
    var_dump($o);
}
try {
    ++$o;
} catch (\TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
    var_dump($o);
}
try {
    --$o;
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
