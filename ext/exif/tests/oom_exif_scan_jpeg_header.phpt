--TEST--
exif_read_data() doesn't reach hard limit when JPEG sections accumulate
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
// Every APP9 section is retained, and a minimal one is only 4 bytes, so the
// retained sections add up to many times the file itself.
$n = (int)((2*1024*1024 - 4) / 4);
$jpeg = "\xFF\xD8" . str_repeat("\xFF\xE9\x00\x02", $n) . "\xFF\xD9";

$file = __DIR__ . '/oom_exif_scan_jpeg_header.jpg';
file_put_contents($file, $jpeg);
unset($jpeg);

var_dump(exif_read_data($file));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/oom_exif_scan_jpeg_header.jpg');
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
