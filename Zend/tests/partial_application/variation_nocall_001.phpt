--TEST--
PFA variation: no call args leak
--FILE--
<?php
function test($a, $b) {}

test(?, new stdClass);

echo "OK";
?>
--EXPECT--
OK
