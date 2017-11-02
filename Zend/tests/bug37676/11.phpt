--TEST--
Bug #37676 (AST Const Access)
--FILE--
<?php
var_dump(null[0][1]);
var_dump(true[0][1]);
?>
--EXPECTF--
Warning: Variable of type null does not accept array offsets in %s on line %d
NULL

Warning: Variable of type boolean does not accept array offsets in %s on line %d
NULL
