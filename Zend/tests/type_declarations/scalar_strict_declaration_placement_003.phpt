--TEST--
Test strict declaration being first operation only 003
--FILE--
hi<?php

declare(strict_types=1);
var_dump(strlen(32));

?>
--EXPECTF--
Fatal error: strict_types declaration must be the very first statement in the script in %s on line %d
