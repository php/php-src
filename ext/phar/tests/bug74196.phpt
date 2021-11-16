--TEST--
PHP bug #74196: PharData->decompress() does not correctly support dot names
--EXTENSIONS--
phar
zlib
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.1.2.3.phar.tar.gz';
$decompressed_name = str_replace( ".gz", "",  $fname);
var_dump(file_exists($fname));

$phar = new Phar($fname);
$phar->decompress();
var_dump(file_exists($decompressed_name));
unlink($decompressed_name);

?>
--EXPECT--
bool(true)
bool(true)
