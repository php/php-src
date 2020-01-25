--TEST--
Can call internal functions from global constants
--FILE--
<?php
const NIL = var_export(null, true);

echo "NIL is " . NIL . "\n";
?>
--EXPECT--
NIL is NULL
