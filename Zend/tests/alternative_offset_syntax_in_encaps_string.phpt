--TEST--
Alternative offset syntax should emit only E_COMPILE_ERROR in string interpolation
--FILE--
<?php "{$g{'h'}}"; ?>
--EXPECTF--
Parse error: syntax error, unexpected token "{", expecting "->" or "?->" or "[" in %s on line %d
