--TEST--
Bug #81684: ??= on $GLOBALS produces an invalid opcode
--FILE--
<?php
$GLOBALS['x'] ??= 'x'; // Fatal error: Invalid opcode 23/1/0
var_dump($GLOBALS['x']);
echo "Done.\n";
?>
--EXPECT--
string(1) "x"
Done.
