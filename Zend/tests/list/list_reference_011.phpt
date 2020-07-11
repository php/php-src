--TEST--
"Reference Unpacking - Compile Error (const)" list()
--FILE--
<?php
const FOO = 10;
[&$f] = FOO;
?>
--EXPECTF--
Fatal error: Cannot assign reference to non referencable value in %s on line %d
