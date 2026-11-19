--TEST--
Interface delegation: empty interface
--FILE--

<?php

interface A {}

class C implements A by $a {}

var_dump(new C);
?>

--EXPECTF--
object(C)#%d (0) {
}
