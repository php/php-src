--TEST--
ZipArchive mutators throw when called from a cancel callback during close()
--EXTENSIONS--
zip
--SKIPIF--
<?php
if (!method_exists(ZipArchive::class, 'registerCancelCallback')) {
    die('skip cancel callbacks are not supported');
}
?>
--FILE--
<?php
$filename = __DIR__ . '/zip_close_mutator_reentry_cancel.zip';
$zip = new ZipArchive();
$zip->open($filename, ZipArchive::CREATE | ZipArchive::OVERWRITE);
$zip->addFromString('a.txt', str_repeat('a', 100000));
$zip->addFromString('b.txt', str_repeat('b', 100000));
$zip->registerCancelCallback(function () use ($zip) {
    static $done = false;
    if ($done) {
        return 0;
    }
    $done = true;
    $mutators = [
        'deleteIndex' => fn() => $zip->deleteIndex(0),
        'addFromString' => fn() => $zip->addFromString('c.txt', 'c'),
        'registerCancelCallback' => fn() => $zip->registerCancelCallback(fn() => 0),
    ];
    foreach ($mutators as $name => $mutator) {
        try {
            var_dump($mutator());
        } catch (Error $e) {
            echo $name, ': ', $e::class, ': ', $e->getMessage(), PHP_EOL;
        }
    }
    return 0;
});
var_dump($zip->close());
var_dump($zip->open($filename, ZipArchive::CHECKCONS));
var_dump($zip->numFiles);
var_dump($zip->getFromName('a.txt') === str_repeat('a', 100000));
var_dump($zip->getFromName('b.txt') === str_repeat('b', 100000));
$zip->close();
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/zip_close_mutator_reentry_cancel.zip');
?>
--EXPECT--
deleteIndex: Error: Already being closed
addFromString: Error: Already being closed
registerCancelCallback: Error: Already being closed
bool(true)
bool(true)
int(2)
bool(true)
bool(true)
