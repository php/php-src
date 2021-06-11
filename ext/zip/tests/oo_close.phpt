--TEST--
zip::close() function
--EXTENSIONS--
zip
--FILE--
<?php

$dirname = __DIR__ . '/';
$zip = new ZipArchive;
if (!$zip->open($dirname . 'test.zip')) {
    exit('failed');
}

if ($zip->status == ZIPARCHIVE::ER_OK) {
    $zip->close();
    echo "ok\n";
} else {
    echo "failed\n";
}
?>
--EXPECT--
ok
