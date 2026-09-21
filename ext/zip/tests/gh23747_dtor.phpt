--TEST--
GH-23747 (Re-entrant operations during ZipArchive destruction are rejected)
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
$filename = __DIR__ . '/gh23747_dtor.zip';

foreach ([
    ['close', []],
    ['getStream', ['f0.txt']],
    ['getStreamName', ['f0.txt']],
    ['getStreamIndex', [0]],
] as [$method, $arguments]) {
    $zip = new ZipArchive();
    $zip->open($filename, ZipArchive::CREATE | ZipArchive::OVERWRITE);
    for ($index = 0; $index < 64; $index++) {
        $zip->addFromString("f$index.txt", str_repeat('x', 2000));
    }

    $weak = WeakReference::create($zip);
    $callback = static function (float $rate) use ($weak, $method, $arguments): void {
        static $done = false;
        if ($done) {
            return;
        }
        $done = true;

        try {
            $weak->get()->$method(...$arguments);
        } catch (Error $error) {
            echo $method, ': ', $error->getMessage(), "\n";
        }
    };
    $zip->registerProgressCallback(0.0, $callback);
    unset($zip);
    echo "destroyed\n";
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh23747_dtor.zip');
?>
--EXPECT--
close: Already being closed
destroyed
getStream: Already being closed
destroyed
getStreamName: Already being closed
destroyed
getStreamIndex: Already being closed
destroyed