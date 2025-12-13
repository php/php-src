--TEST--
Type alias name cannot be used twice
--FILE--
<?php
use type int as Number;
use type float as Number;
?>
--EXPECTF--
Fatal error: Cannot use type alias 'Number' because the name is already in use in %s on line %d
