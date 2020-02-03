--TEST--
__HALT_COMPILER(); bad define() of __COMPILER_HALT_OFFSET__ 2
--FILE--
<?php

define('__COMPILER_HALT_OFFSET__', 1);
__halt_compiler();
?>

?>
==DONE==
--EXPECTF--
Notice: Constant __COMPILER_HALT_OFFSET__ already defined in %s on line %d
