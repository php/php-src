--TEST--
Bug #80184: Complex expression in while / if statements resolves to false incorrectly
--FILE--
<?php

$callbacks = [
	function () { echo "First item!\n"; },
	function () { echo "Second item!\n"; },
	function () { echo "Third item!\n"; },
	function () { echo "Fourth item!\n"; },
];

while ($callback = array_shift($callbacks) and ($callback() || true));

?>
--EXPECT--
First item!
Second item!
Third item!
Fourth item!
