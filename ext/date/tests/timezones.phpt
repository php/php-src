--TEST--
setting bogus timezones
--FILE--
<?php

//bogus
var_dump(date_default_timezone_set('AAA'));
var_dump(date_default_timezone_set('ZZZ'));


//now the first and the last one
var_dump(date_default_timezone_set("Africa/Abidjan"));
var_dump(date_default_timezone_set("Zulu"));

echo "done\n";

?>
--EXPECTF--
Notice: date_default_timezone_set(): Timezone ID 'AAA' is invalid in %stimezones.php on line 4
bool(false)

Notice: date_default_timezone_set(): Timezone ID 'ZZZ' is invalid in %stimezones.php on line 5
bool(false)
bool(true)
bool(true)
done
