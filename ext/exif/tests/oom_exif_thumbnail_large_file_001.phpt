--TEST--
exif_thumbnail() doesn't reach hard limit when the thumbnail is larger than the memory limit
--EXTENSIONS--
exif
--INI--
memory_limit=8M
--FILE--
<?php
require __DIR__ . '/oom_exif_big_thumbnail.inc';

$file = __DIR__ . '/oom_exif_thumbnail_large_file_001.tif';
oom_exif_write_big_thumbnail($file, 12 * 1024 * 1024);

var_dump(exif_thumbnail($file));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/oom_exif_thumbnail_large_file_001.tif');
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
