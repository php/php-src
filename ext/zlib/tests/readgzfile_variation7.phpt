--TEST--
readgzfile() with unknown file
--EXTENSIONS--
zlib
--FILE--
<?php

$file = "unknown_file.txt.gz";
var_dump(readgzfile($file, false));
var_dump(readgzfile($file, true));
?>
--EXPECTF--
Warning: readgzfile(unknown_file.txt.gz): Failed to open stream: No such file or directory in %s on line %d
bool(false)

Warning: readgzfile(unknown_file.txt.gz): Failed to open stream: No such file or directory in %s on line %d
bool(false)
