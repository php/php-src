--TEST--
__HALT_COMPILER();
--FILE--
<?php echo 'test'; var_dump(__COMPILER_HALT_OFFSET__); __HALT_COMPILER();
?>
===DONE===
--EXPECT--
testint(73)