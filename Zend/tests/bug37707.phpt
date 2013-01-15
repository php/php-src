--TEST--
Bug #37707 (clone without assigning leaks memory)
--FILE--
<?php
class testme {
	function __clone() {
		echo "clonned\n";
	}
}
clone new testme();
echo "NO LEAK\n";
?>
--EXPECT--
clonned
NO LEAK

