--TEST--
Test function quotemeta() - using an empty string is given as str.
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
User Group: PHPSP #PHPTestFestBrasil
--FILE--
<?php
$str = "";
var_dump(quotemeta($str));
?>
--EXPECT--
bool(false)
