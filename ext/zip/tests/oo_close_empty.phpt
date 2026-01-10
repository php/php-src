--TEST--
Close empty file behavior
--EXTENSIONS--
zip
--SKIPIF--
<?php
if (version_compare(ZipArchive::LIBZIP_VERSION, '1.10.0', '<')) die('skip libzip < 1.10.0');
?>
--FILE--
<?php
$name = __DIR__ . '/oo_close_empty.zip';

function run($name, $keep) {
    copy(__DIR__ . '/test.zip', $name);

    $zip = new ZipArchive();
    $zip->open($name, ZIPARCHIVE::CREATE);
	if ($keep) {
        echo "\nClose and keep\n";
        var_dump($zip->setArchiveFlag(ZipArchive::AFL_CREATE_OR_KEEP_FILE_FOR_EMPTY_ARCHIVE, 1), $zip->status === ZipArchive::ER_OK);
    } else {
        echo "Close and delete\n";
    }
    var_dump($zip->getArchiveFlag(ZipArchive::AFL_CREATE_OR_KEEP_FILE_FOR_EMPTY_ARCHIVE));
    for($i=$zip->numFiles ; $i ;) {
        $zip->deleteIndex(--$i);
    }
    $zip->close();
    var_dump(file_exists($name));
 }
run($name, false);
run($name, true);
?>
--CLEAN--
<?php
$name = __DIR__ . '/oo_close_empty.zip';
@unlink($name);
?>
--EXPECTF--
Close and delete
int(0)
bool(false)

Close and keep
bool(true)
bool(true)
int(1)
bool(true)
