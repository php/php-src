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
    $callbackState = new stdClass;
    $callbackStateRef = WeakReference::create($callbackState);
    if ($type === 'progress') {
        var_dump($zip->registerProgressCallback(0.5, static function (float $rate) use ($callbackState): void {}));
    } else {
        var_dump($zip->registerCancelCallback(static function () use ($callbackState): int { return 0; }));
    }
    $zip->addFromString("$type.txt", 'late');
    $stream = $zip->getStream('entry.txt');
    if (!is_resource($stream)) {
        throw new Exception('Failed to open entry stream');
    }
    $weakRef = WeakReference::create($zip);
    unset($callbackState, $zip);

    var_dump($weakRef->get());
    var_dump($callbackStateRef->get() !== null);
    var_dump(stream_get_contents($stream));
    fclose($stream);
    var_dump($callbackStateRef->get());
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
bool(true)
string(8) "contents"
NULL
progress done
bool(true)
NULL
bool(true)
string(8) "contents"
NULL
cancel done
