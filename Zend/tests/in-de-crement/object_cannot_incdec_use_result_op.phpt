--TEST--
Cannot increment/decrement objects
--FILE--
<?php
class Foo { }
$o = new Foo;

try {
    $y = $o++;
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
    var_dump($o);
}
try {
    $y = $o--;
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
    var_dump($o);
}
try {
    $y = ++$o;
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
    var_dump($o);
}
try {
    $y = --$o;
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
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
