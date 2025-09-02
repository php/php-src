--TEST--
finfo_open(): Testing magic_file names
--EXTENSIONS--
fileinfo
--INI--
open_basedir='.'
--FILE--
<?php
$buggyPath = str_repeat('a', PHP_MAXPATHLEN + 1);

var_dump(finfo_open(FILEINFO_MIME, $buggyPath));

try {
    $object = new finfo(FILEINFO_MIME, $buggyPath);
} catch (\Exception $ex) {
    echo "TEST:" . $ex->getMessage() . PHP_EOL;
}
?>
--EXPECTF--
Warning: finfo_open(): File name is longer than the maximum allowed path length on this platform (%d): %s in %s on line %d
bool(false)
TEST:finfo::__construct(): File name is longer than the maximum allowed path length on this platform (%d): %s
