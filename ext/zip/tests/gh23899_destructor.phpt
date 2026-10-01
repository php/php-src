--TEST--
GH-23899 (Cancel callback return value whose destructor throws during shutdown)
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
class ThrowingDestructor {
    public function __destruct() {
        throw new Exception('destructor');
    }
}

$zip = new ZipArchive;
$zip->open(__DIR__ . '/gh23899_destructor.zip', ZipArchive::CREATE | ZipArchive::OVERWRITE);
$zip->registerCancelCallback(function () {
    return new ThrowingDestructor;
});
$zip->addFromString('test', 'test');
echo "Done\n";
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh23899_destructor.zip');
?>
--EXPECTF--
Done

Fatal error: Uncaught TypeError: Return value of callback provided to ZipArchive::registerCancelCallback() must be of type int, ThrowingDestructor returned in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
