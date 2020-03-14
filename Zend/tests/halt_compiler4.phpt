--TEST--
__HALT_COMPILER(); bad define() of __COMPILER_HALT_OFFSET__ 2
--FILE--
<?php
require __DIR__ . '/constants_helpers.inc';

tchelper_define('__COMPILER_HALT_OFFSET__', 1);

__HALT_COMPILER();
?>

==DONE==
--EXPECT--
>> define("__COMPILER_HALT_OFFSET__", integer);
ValueError :: Constant __COMPILER_HALT_OFFSET__ already defined
