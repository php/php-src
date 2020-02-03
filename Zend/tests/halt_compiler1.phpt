--TEST--
__HALT_COMPILER();
--FILE--
<?php

echo 'test';
var_dump(__COMPILER_HALT_OFFSET__);
__halt_compiler();
?>

?>
===DONE===
--EXPECT--
testint(73)
