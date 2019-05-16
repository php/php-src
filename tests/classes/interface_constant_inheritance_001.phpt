--TEST--
Ensure an interface may not shadow an inherited constant.
--FILE--
<?php
interface I1 {
	const FOO = 10;
}

interface I2 extends I1 {
	const FOO = 10;
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Cannot inherit previously-inherited or override constant FOO from interface I1 in %s on line 6
