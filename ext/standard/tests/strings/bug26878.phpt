--TEST--
Bug #26878 (problem with multiple references to the same variable with different types)
--FILE--
<?php
	printf('Int: %1$d and as string: %1$s', 'some string');
	echo "\n";
?>
--EXPECT--
Int: 0 and as string: some string
