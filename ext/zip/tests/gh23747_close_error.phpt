--TEST--
GH-23747 (Re-entrant close from a ZipArchive close warning is rejected)
--EXTENSIONS--
zip
--FILE--
<?php
$filename = __DIR__ . '/gh23747_close_error.zip';
$source = __DIR__ . '/gh23747_close_error.txt';

foreach (['close', 'destruct'] as $operation) {
    echo $operation, ":\n";
    file_put_contents($source, 'contents');
    $zip = new ZipArchive();
    $zip->open($filename, ZipArchive::CREATE | ZipArchive::OVERWRITE);
    $zip->addFile($source, 'file.txt');
    unlink($source);

    $weak = WeakReference::create($zip);
    set_error_handler(static function (int $errno, string $message) use ($weak): bool {
        try {
            $weak->get()->close();
        } catch (Error $error) {
            echo $error::class, ': ', $error->getMessage(), "\n";
        }
        return true;
    });

    if ($operation === 'close') {
        var_dump($zip->close());
    } else {
        unset($zip);
        echo "destroyed\n";
    }
    restore_error_handler();
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh23747_close_error.zip');
@unlink(__DIR__ . '/gh23747_close_error.txt');
?>
--EXPECT--
close:
Error: Already being closed
bool(false)
destruct:
Error: Already being closed
destroyed