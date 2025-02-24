--TEST--
Bug #53305 (E_NOTICE when defining a constant starts with __COMPILER_HALT_OFFSET__)
--FILE--
<?php
error_reporting(E_ALL);

define('__COMPILER_HALT_OFFSET__1', 1);
define('__COMPILER_HALT_OFFSET__2', 2);
define('__COMPILER_HALT_OFFSET__', 3);
define('__COMPILER_HALT_OFFSET__1'.chr(0), 4);

var_dump(__COMPILER_HALT_OFFSET__1);
var_dump(constant('__COMPILER_HALT_OFFSET__1'.chr(0)));

?>
--EXPECTF--
Warning: Constant __COMPILER_HALT_OFFSET__ already defined in %s on line %d
int(1)
int(4)
