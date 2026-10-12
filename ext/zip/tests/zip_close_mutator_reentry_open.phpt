--TEST--
ZipArchive mutators throw when open() implicitly closes the previous archive
--EXTENSIONS--
zip
--SKIPIF--
<?php
if (!method_exists(ZipArchive::class, 'registerProgressCallback')) {
    die('skip progress callbacks are not supported');
}
?>
--FILE--
<?php
$filename = __DIR__ . '/zip_close_mutator_reentry_open.zip';
$nextFilename = __DIR__ . '/zip_close_mutator_reentry_open_next.zip';
$zip = new ZipArchive();
$zip->open($filename, ZipArchive::CREATE | ZipArchive::OVERWRITE);
$zip->addFromString('a.txt', str_repeat('a', 100000));
$zip->addFromString('b.txt', str_repeat('b', 100000));
$zip->registerProgressCallback(0.0, function ($rate) use ($zip) {
    static $done = false;
    if ($done || $rate <= 0) {
        return;
    }
    $done = true;
    $mutators = [
        'deleteIndex' => fn() => $zip->deleteIndex(0),
        'addFromString' => fn() => $zip->addFromString('c.txt', 'c'),
        'registerProgressCallback' => fn() => $zip->registerProgressCallback(0.5, function () {}),
    ];
    foreach ($mutators as $name => $mutator) {
        try {
            var_dump($mutator());
        } catch (Error $e) {
            echo $name, ': ', $e::class, ': ', $e->getMessage(), PHP_EOL;
        }
    }
});
var_dump($zip->open($nextFilename, ZipArchive::CREATE | ZipArchive::OVERWRITE));
var_dump($zip->addFromString('new.txt', 'new contents'));
var_dump($zip->close());
var_dump($zip->open($filename, ZipArchive::CHECKCONS));
var_dump($zip->numFiles);
var_dump($zip->getFromName('a.txt') === str_repeat('a', 100000));
var_dump($zip->getFromName('b.txt') === str_repeat('b', 100000));
$zip->close();
var_dump($zip->open($nextFilename, ZipArchive::CHECKCONS));
var_dump($zip->numFiles);
var_dump($zip->getFromName('new.txt') === 'new contents');
$zip->close();
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/zip_close_mutator_reentry_open.zip');
@unlink(__DIR__ . '/zip_close_mutator_reentry_open_next.zip');
?>
--EXPECT--
deleteIndex: Error: Already being closed
addFromString: Error: Already being closed
registerProgressCallback: Error: Already being closed
bool(true)
bool(true)
bool(true)
bool(true)
int(2)
bool(true)
bool(true)
bool(true)
int(1)
bool(true)
