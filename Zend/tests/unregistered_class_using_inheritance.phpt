--TEST--
Unused class extending/implementing/using should not leak
--FILE--
<?php

if (0) {
    class A extends B implements C { use D; }
}

?>
===DONE===
--EXPECT--
===DONE===
