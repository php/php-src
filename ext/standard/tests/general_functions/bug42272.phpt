--TEST--
Bug #42272: var_export() incorrectly escapes char(0).
--FILE--
<?php
$foo = var_export("\0", true );
echo $foo, "\n";
var_export("a\0b");
?>
--EXPECT--
'' . "\0" . ''
'a' . "\0" . 'b'
