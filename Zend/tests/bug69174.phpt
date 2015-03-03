--TEST--
Bug #69174 (leaks when unused inner class use traits precedence)
--FILE--
<?php
function test() {
	class C1 {
		use T1, T2 {
			T1::foo insteadof T2;
		}
	}
}

echo "DONE\n";
?>
--EXPECT--
DONE