--TEST--
getimagesizefromstring() IFF chunk size integer overflow (GH-getimagesize_oflow)
--CREDITS--
Alexandre Daubois
--FILE--
<?php
// IFF/ILBM with a chunk size of INT_MAX (0x7fffffff), an odd value.
// The parser rounds odd chunk sizes up to even via size++, which overflowed
// when size == INT_MAX. It must be handled gracefully rather than triggering UB.
$payload = "FORM" . "\x00\x00\x00\x00" . "ILBM" . "ABCD" . "\x7f\xff\xff\xff";
var_dump(getimagesizefromstring($payload));
?>
--EXPECT--
bool(false)
