--TEST--
Bug #81726 (phar wrapper: DOS when using quine gzip file)
--EXTENSIONS--
phar
zlib
--FILE--
<?php
var_dump(fopen("phar://" . __DIR__ . "/bug81726.gz", "r"));
?>
--EXPECTF--
Warning: fopen(phar://%s): Failed to open stream: unable to decompress gzipped phar archive "%s" in %s on line %d
bool(false)
