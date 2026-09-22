--TEST--
ZipArchive::addFromString() buffers are released when the archive is closed
--EXTENSIONS--
zip
--FILE--
<?php
$filename = __DIR__ . '/oo_addfromstring_reopen_memory.zip';
$blob = str_repeat('q', 200000);

$zip = new ZipArchive;
$start = memory_get_usage();

for ($i = 0; $i < 50; $i++) {
    $zip->open($filename, ZipArchive::CREATE | ZipArchive::OVERWRITE);
    $zip->addFromString('entry.txt', $blob);
    $zip->close();
}

var_dump(memory_get_usage() - $start < 1000000);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/oo_addfromstring_reopen_memory.zip');
?>
--EXPECT--
bool(true)
