--TEST--
Bug #74922 (Composed class has fatal error with duplicate, equal const properties)
--FILE--
<?php

trait T {
    public $x = self::X;
}
trait T2 {
    public $x = self::X;
}
class C {
    use T, T2;
    const X = 42;
}
var_dump((new C)->x);

?>
--EXPECT--
int(42)
