--TEST--
exif_read_data() doesn't reach hard limit reading a thumbnail larger than the hard limit
--EXTENSIONS--
exif
--INI--
memory_limit=8M
--FILE--
<?php
require __DIR__ . '/oom_exif_big_thumbnail.inc';

// Here the thumbnail is larger than the hard limit on its own
$file = __DIR__ . '/oom_exif_thumbnail_large_file_002.tif';
oom_exif_write_big_thumbnail($file, 20 * 1024 * 1024);

var_dump(exif_read_data($file, null, false, true));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/oom_exif_thumbnail_large_file_002.tif');
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
