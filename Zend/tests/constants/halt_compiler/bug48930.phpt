--TEST--
Bug #48930 (__COMPILER_HALT_OFFSET__ incorrect in PHP>=5.3)
--FILE--
#!php
<?php

/*
 * Test
 */
printf("__COMPILER_HALT_OFFSET__ is %d\n",__COMPILER_HALT_OFFSET__);

__halt_compiler();

?>
--EXPECT--
__COMPILER_HALT_OFFSET__ is 116
