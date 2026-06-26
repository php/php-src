--TEST--
Static methods: trait type parameter is allowed in a static method signature
--FILE--
<?php
trait Bag<T> {
    public static function make(T $x): void {}
}
echo "ok\n";
?>
--EXPECT--
ok
