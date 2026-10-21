--TEST--
exif_read_data() doesn't reach hard limit when tag values accumulate
--EXTENSIONS--
exif
--INI--
memory_limit=8M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--FILE--
<?php
// Each IFD entry is 12 bytes but claims a 256 KB value, so the retained
// values add up to far more than the file itself.
$n = 1000;
$payload_len = 256 * 1024;
$payload_off = 8 + 2 + $n*12 + 4;

$entries = '';
for ($i = 0; $i < $n; $i++) {
    $entries .= pack('vvVV', 0x9000 + ($i & 0xFF), 1, $payload_len, $payload_off);
}
$t = "II*\x00" . pack('V', 8)
   . pack('v', $n) . $entries . pack('V', 0)
   . str_repeat("A", $payload_len);

$file = __DIR__ . '/oom_exif_read_data_tags.tif';
file_put_contents($file, $t);
unset($entries, $t);

var_dump(exif_read_data($file, null, true));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/oom_exif_read_data_tags.tif');
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
