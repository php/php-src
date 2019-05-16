--TEST--
Ensure a class may not inherit two constants with the same name from two separate interfaces.
--FILE--
<?php
interface I1 {
	const FOO = 10;
}

interface I2 {
	const FOO = 10;
}

class C implements I1,I2 {
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Cannot inherit previously-inherited or override constant FOO from interface I2 in %s on line 10
