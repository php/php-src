--TEST--
Generic syntax: turbofish on static method call
--FILE--
<?php
class C {
    public static function s<A, B = mixed>($x) { return $x * 2; }
}
echo C::s::<int>(5), "\n";
echo C::s::<int, string>(7), "\n";
?>
--EXPECT--
10
14
