--TEST--
Test bindec() - basic function test bindec()
--FILE--
<?php
bindec();
bindec('01010101111',true);
?>
--EXPECTF--

Warning: Wrong parameter count for bindec() in %s on line 2

Warning: Wrong parameter count for bindec() in %s on line 3
