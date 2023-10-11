--TEST--
phpdbg_end_oplog() alone must not crash
--PHPDBG--
r
q
--EXPECTF--
[Successful compilation of %s]
prompt> 
Warning: Cannot end an oplog without starting it in %s on line 3
NULL
[Script ended normally]
prompt> 
--FILE--
<?php

var_dump(phpdbg_end_oplog());
?>
