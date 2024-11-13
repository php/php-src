--TEST--
Leading comma in keyed list assignment
--FILE--
<?php

[, "a" => $b] = [1, "a" => 2];

?>
--EXPECTF--
Fatal error: Cannot use empty array entries in keyed array assignment in %s on line %d
