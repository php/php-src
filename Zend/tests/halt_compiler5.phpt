--TEST--
Using __COMPILER_HALF_OFFSET__ with trailing {} (OSS-Fuzz #17895)
--FILE--
<?php
__COMPILER_HALT_OFFSET__;
{}
--EXPECTF--
Warning: Use of undefined constant __COMPILER_HALT_OFFSET__ - assumed '__COMPILER_HALT_OFFSET__' (this will throw an Error in a future version of PHP) in %s on line %d
