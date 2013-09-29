--TEST--
declare bare anonymous class
--FILE--
<?php
var_dump(new class{});
?>
--EXPECTF--	
object(%s$$1)#%d (0) {
}

