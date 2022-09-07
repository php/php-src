--TEST--
Alternative offset syntax should emit only E_COMPILE_ERROR in string interpolation
--FILE--
<?php "{$g{'h'}}"; ?>
--EXPECTF--
Fatal error: Array and string offset access syntax with curly braces is no longer supported in %s on line 1
