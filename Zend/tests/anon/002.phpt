--TEST--
declare anonymous class extending another
--FILE--
<?php
var_dump(new class extends stdClass{});
?>
--EXPECTF--	
object(%s$$1)#%d (0) {
}

