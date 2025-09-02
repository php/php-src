--TEST--
GH-10251 (Assertion `(flag & (1<<3)) == 0' failed.)
--FILE--
<?php
#[\AllowDynamicProperties]
class A
{
    function __set($o, $l)
    {
        $this->$p = $v;
    }
}
$a = new A();
$pp = "";
$op = $pp & "";
// Bitwise operators on strings don't compute the hash.
// The code below previously assumed a hash was actually computed, leading to a crash.
$a->$op = 0;
echo "Done\n";
?>
--EXPECTF--
Warning: Undefined variable $v in %s on line %d

Warning: Undefined variable $p in %s on line %d
Done
