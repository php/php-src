--TEST--
Cannot increment/decrement objects
--FILE--
<?php
class Foo { }
$o = new Foo;

try {
    $o++;
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    var_dump($o);
}
try {
    $o--;
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    var_dump($o);
}
try {
    ++$o;
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    var_dump($o);
}
try {
    --$o;
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    var_dump($o);
}
?>
--EXPECT--
TypeError: Cannot increment Foo
object(Foo)#1 (0) {
}
TypeError: Cannot decrement Foo
object(Foo)#1 (0) {
}
TypeError: Cannot increment Foo
object(Foo)#1 (0) {
}
TypeError: Cannot decrement Foo
object(Foo)#1 (0) {
}
