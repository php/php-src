--TEST--
declare bare anonymous class
--FILE--
<?php
var_dump(new class{});
?>
--EXPECTF--
object(class@%s)#%d (0) {
}
