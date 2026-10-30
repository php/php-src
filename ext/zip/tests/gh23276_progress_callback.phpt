--TEST--
GH-23276 (ZipArchive progress callback outlives the object while a stream holds the archive)
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
$filename = __DIR__ . '/gh23276_progress_callback.zip';

$zip = new ZipArchive;
$zip->open($filename, ZipArchive::CREATE | ZipArchive::OVERWRITE);
$zip->addFromString('entry.txt', 'contents');
$zip->close();

$zip->open($filename);
$callbackState = new stdClass;
$callbackStateRef = WeakReference::create($callbackState);
var_dump($zip->registerProgressCallback(
    0.5,
    static function (float $rate) use ($callbackState): void {},
));
$zip->addFromString('progress.txt', 'late');
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
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh23276_progress_callback.zip');
?>
--EXPECT--
bool(true)
NULL
bool(true)
string(8) "contents"
NULL
