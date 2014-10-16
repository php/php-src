--TEST--
declare bare anonymous class
--FILE--
<?php
var_dump(new class{});
?>
--EXPECTF--	
object(%s@%d)#%d (0) {
}


