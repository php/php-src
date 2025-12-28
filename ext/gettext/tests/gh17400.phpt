--TEST--
GH-17400 bindtextdomain segfaults with invalid domain/domain with null bytes.
--EXTENSIONS--
gettext
--CREDITS--
YuanchengJiang
--FILE--
<?php
$utf16_first_le = pack("H*", "00d800dc");
$utf16le_char_bad = pack("H*", "00dc00dc");

try {
	bindtextdomain($utf16le_char_bad,$utf16_first_le);
} catch (\ValueError $e) {
	echo $e->getMessage();
}
?>
--EXPECT--
bindtextdomain(): Argument #1 ($domain) must not contain any null bytes
