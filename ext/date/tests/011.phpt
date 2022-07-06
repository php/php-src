--TEST--
timezone_name_from_abbr() tests
--FILE--
<?php
date_default_timezone_set('UTC');

var_dump(timezone_name_from_abbr("CET"));
var_dump(timezone_name_from_abbr("AXST"));
var_dump(timezone_name_from_abbr("", 3600));
var_dump(timezone_name_from_abbr("", 3600, 0));

echo "Done\n";
?>
--EXPECT--
string(13) "Europe/Berlin"
bool(false)
bool(false)
string(12) "Europe/Paris"
Done
