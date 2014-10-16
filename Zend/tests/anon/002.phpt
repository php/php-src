--TEST--
declare anonymous class extending another
--FILE--
<?php
var_dump(new class extends stdClass{});
?>
--EXPECTF--	
object(%s@%d)#%d (0) {
}

