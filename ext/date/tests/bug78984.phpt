--TEST--
Bug #78984 (DateTimeZone accepting invalid UTC timezones)
--FILE--
<?php
$tz = @timezone_open('+30157');
if ($tz) {
    // relevant case for quick fix
    var_dump($tz->getName());
} else {
    // dummy case for proper fix
    var_dump("+00:00");
}
?>
--EXPECT--
string(6) "+00:00"
