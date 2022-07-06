--TEST--
"Reference Unpacking - Compile Error (scalar)" list()
--FILE--
<?php
list(&$foo) = [42];
?>
--EXPECTF--
Fatal error: Cannot assign reference to non referencable value in %s on line %d
