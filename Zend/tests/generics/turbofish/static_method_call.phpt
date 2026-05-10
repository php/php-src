--TEST--
Turbofish: static method call
--FILE--
<?php
class C {
    static function m<T>($x) { return $x; }
}
var_dump(C::m::<int>(7));
?>
--EXPECT--
int(7)
