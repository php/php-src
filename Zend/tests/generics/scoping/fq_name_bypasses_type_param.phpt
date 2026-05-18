--TEST--
Scoping: a fully qualified name `\T` bypasses the type-parameter rule
--FILE--
<?php
class T {
    public int $v = 5;
}
function f<T>(): int {
    $x = new \T();
    return $x->v;
}
echo f(), "\n";
?>
--EXPECT--
5
