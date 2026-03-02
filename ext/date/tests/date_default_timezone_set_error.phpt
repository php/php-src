--TEST--
Test date_default_timezone_set() function : error variations
--FILE--
<?php
echo "*** Testing date_default_timezone_set() : error variations ***\n";

echo "\n-- Testing date_default_timezone_set() function with invalid timezone identifier  --\n";
var_dump( date_default_timezone_set("foo") );

?>
--EXPECTF--
*** Testing date_default_timezone_set() : error variations ***

-- Testing date_default_timezone_set() function with invalid timezone identifier  --

Notice: date_default_timezone_set(): Timezone ID 'foo' is invalid in %s on line %d
bool(false)
