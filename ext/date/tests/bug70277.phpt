--TEST--
Bug #70277 (new DateTimeZone($foo) is ignoring text after null byte)
--FILE--
<?php
$timezone = "Europe/Zurich\0Foo";
try {
    var_dump(timezone_open($timezone));
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump(new DateTimeZone($timezone));
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: timezone_open(): Argument #1 ($timezone) must not contain any null bytes
ValueError: DateTimeZone::__construct(): Argument #1 ($timezone) must not contain any null bytes
