--TEST--
Test hrtime() errors
--FILE--
<?php

hrtime(true);

?>
--EXPECTF--
Warning: hrtime() expects exactly 0 parameters, 1 given in %s
