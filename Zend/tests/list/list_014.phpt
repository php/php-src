--TEST--
Cannot destructure using array(), even if nested
--FILE--
<?php

[array($a)] = [array(42)];
var_dump($a);

?>
--EXPECTF--
Fatal error: Cannot assign to array(), use [] instead in %s on line %d
