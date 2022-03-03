--TEST--
zip::open() function
--EXTENSIONS--
zip
--FILE--
<?php

$dirname = __DIR__ . '/';
$zip = new ZipArchive;
$r = $zip->open($dirname . 'nofile');
if ($r !== TRUE) {
    echo "ER_OPEN: ok\n";
} else {
    echo "ER_OPEN: FAILED\n";
}

$r = $zip->open($dirname . 'nofile', ZIPARCHIVE::CREATE);
if (!$r) {
    echo "create: failed\n";
} else {
    echo "create: ok\n";
}
@unlink($dirname . 'nofile');

$zip = new ZipArchive;
try {
    $zip->open('');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

if (!$zip->open($dirname . 'test.zip')) {
    exit("failed 1\n");
}

if ($zip->status == ZIPARCHIVE::ER_OK) {
    echo "OK\n";
} else {
    echo "failed\n";
}
?>
--EXPECT--
ER_OPEN: ok
create: ok
ZipArchive::open(): Argument #1 ($filename) cannot be empty
OK
