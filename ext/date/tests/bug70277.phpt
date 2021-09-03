--TEST--
Bug #70277 (new DateTimeZone($foo) is ignoring text after null byte)
--FILE--
<?php
$timezone = "Europe/Zurich\0Foo";
try {
    var_dump(timezone_open($timezone));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(new DateTimeZone($timezone));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
timezone_open(): Argument #1 ($timezone) must not contain any null bytes
DateTimeZone::__construct(): Argument #1 ($timezone) must not contain any null bytes
