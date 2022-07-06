--TEST--
Ensure a class may not shadow a constant inherited from an interface.
--FILE--
<?php
interface I {
    const FOO = 10;
}

class C implements I {
    const FOO = 10;
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Cannot inherit previously-inherited or override constant FOO from interface I in %s on line 6
