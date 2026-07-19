--TEST--
gzfile() with unknown file
--EXTENSIONS--
zlib
--FILE--
<?php

$filename = "nonexistent_file_gzfile.txt.gz";

var_dump(gzfile($filename, false));
var_dump(gzfile($filename, true));
?>
--EXPECTF--
Warning: gzfile(): Failed to open stream: No such file or directory in %s on line %d
bool(false)

Warning: gzfile(): Failed to open stream: No such file or directory in %s on line %d
bool(false)
