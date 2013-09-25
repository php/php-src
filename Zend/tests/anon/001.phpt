--TEST--
declare bare anonymous class
--FILE--
<?php
var_dump(new class{});
?>
--EXPECTF--	
object(Class$$1)#%d (0) {
}

