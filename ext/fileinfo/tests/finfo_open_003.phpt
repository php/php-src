--TEST--
finfo_open(): Testing magic_file names
--EXTENSIONS--
fileinfo
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
bool(false)
TEST:Constructor failed
