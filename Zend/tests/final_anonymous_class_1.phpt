--TEST--
Final anonymous classes
--FILE--
<?php

$x = new final class {};

echo "OK";

?>
--EXPECTF--
OK
