--TEST--
NIL constant is deprecated
--EXTENSIONS--
imap
--FILE--
<?php
var_dump(NIL);
?>
--EXPECTF--
Deprecated: Constant NIL is deprecated in %s on line %d
int(0)
