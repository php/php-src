--TEST--
Incorrect DCE of ADD_ARRAY_ELEMENT
--FILE--
<?php
[0, "$a" => "$b"];
?>
DONE
--EXPECTF--
Warning: Undefined variable $a in %sdce_011.php on line 2

Warning: Undefined variable $b in %sdce_011.php on line 2
DONE
