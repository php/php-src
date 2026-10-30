--TEST--
GH-23747 (ZipArchive::close() from inside a progress or cancel callback causes segv)
--CREDITS--
djarfluka
--EXTENSIONS--
zip
--SKIPIF--
<?php
if (!method_exists(ZipArchive::class, 'registerProgressCallback')) {
    die('skip progress callbacks are not supported');
}
if (!method_exists(ZipArchive::class, 'registerCancelCallback')) {
    die('skip cancel callbacks are not supported');
}
?>
--FILE--
<?php
function populate(ZipArchive $zip, string $filename): void {
    $zip->open($filename, ZipArchive::CREATE | ZipArchive::OVERWRITE);
    for ($i = 0; $i < 64; $i++) {
        $zip->addFromString("f$i.txt", str_repeat('x', 2000));
    }
}

$filename = __DIR__ . '/gh23747.zip';

$zip = new ZipArchive();
populate($zip, $filename);
$zip->registerProgressCallback(0.0, function ($rate) use ($zip) {
    static $done = false;
    if (!$done) {
        $done = true;
        try {
            $zip->close();
        } catch (Error $e) {
            echo $e::class, ': ', $e->getMessage(), PHP_EOL;
        }
    }
});
var_dump($zip->close());

$zip = new ZipArchive();
populate($zip, $filename);
$zip->registerCancelCallback(function () use ($zip) {
    static $done = false;
    if (!$done) {
        $done = true;
        try {
            $zip->close();
        } catch (Error $e) {
            echo $e::class, ': ', $e->getMessage(), PHP_EOL;
        }
    }
    return 0;
});
var_dump($zip->close());

$zip = new ZipArchive();
populate($zip, $filename);
$zip->registerProgressCallback(0.0, function ($rate) use ($zip) {
    static $done = false;
    if (!$done) {
        $done = true;
        try {
            $zip->close();
        } catch (Error $e) {
            echo $e::class, ': ', $e->getMessage(), PHP_EOL;
        }
    }
});
var_dump($zip->open($filename));
var_dump($zip->count());

$zip = new ZipArchive();
populate($zip, $filename);
$zip->registerProgressCallback(0.0, function ($rate) use ($zip, $filename) {
    static $done = false;
    if (!$done) {
        $done = true;
        try {
            $zip->open($filename);
        } catch (Error $e) {
            echo $e::class, ': ', $e->getMessage(), PHP_EOL;
        }
    }
});
var_dump($zip->close());
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh23747.zip');
?>
--EXPECT--
Error: Already being closed
bool(true)
Error: Already being closed
bool(true)
Error: Already being closed
bool(true)
int(64)
Error: Already being closed
bool(true)
