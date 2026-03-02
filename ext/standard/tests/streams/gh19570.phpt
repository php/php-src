--TEST--
GH-19570 (unable to fseek in /dev/zero and /dev/null)
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== "Linux") die("skip only for Linux");
?>
--FILE--
<?php
$devices = [
    // Note: could test others but they're not necessarily available
    "/dev/zero",
    "/dev/null",
    "/dev/full",
];
foreach ($devices as $device) {
    var_dump(fseek(fopen($device, "rb"), 1*1024*1024, SEEK_SET));
}
?>
--EXPECT--
int(0)
int(0)
int(0)
