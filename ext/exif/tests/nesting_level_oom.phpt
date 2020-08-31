--TEST--
Should not cause OOM
--FILE--
<?php

var_dump(@exif_read_data(__DIR__ . '/nesting_level_oom.tiff'));

?>
--EXPECT--
bool(false)
