--TEST--
GH-23276 (ZipArchive callbacks outlive the object while a stream holds the archive)
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
$filename = __DIR__ . '/gh23276_callbacks.zip';

$zip = new ZipArchive;
$zip->open($filename, ZipArchive::CREATE | ZipArchive::OVERWRITE);
$zip->addFromString('entry.txt', 'contents');
$zip->close();

function testCallback(string $filename, string $type): void {
    $zip = new ZipArchive;
    $zip->open($filename);
    if ($type === 'progress') {
        var_dump($zip->registerProgressCallback(0.5, static function (float $rate): void {}));
    } else {
        var_dump($zip->registerCancelCallback(static function (): int { return 0; }));
    }
    $zip->addFromString("$type.txt", 'late');
    $stream = $zip->getStream('entry.txt');
    $weakRef = WeakReference::create($zip);
    unset($zip);

    var_dump($weakRef->get());
    var_dump(stream_get_contents($stream));
    fclose($stream);
    echo "$type done\n";
}

testCallback($filename, 'progress');
testCallback($filename, 'cancel');
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh23276_callbacks.zip');
?>
--EXPECT--
bool(true)
NULL
string(8) "contents"
progress done
bool(true)
NULL
string(8) "contents"
cancel done
