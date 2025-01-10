--TEST--
ZipArchive::registerCancelCallback() with a trampoline
--EXTENSIONS--
zip
--SKIPIF--
<?php
/* $Id$ */
if (!method_exists('ZipArchive', 'registerCancelCallback')) die('skip libzip too old');
?>
--INI--
date.timezone=UTC
--FILE--
<?php
$dirname = dirname(__FILE__) . '/';
$file = $dirname . '__tmp_oo_cancel_trampoline.zip';

class TrampolineTest {
    public function __call(string $name, array $arguments): int {
        echo 'Trampoline for ', $name, PHP_EOL;
        if ($name === 'trampolineThrow') {
            throw new Exception('boo');
        }
        var_dump($arguments);
        return -1;
    }
}

$o = new TrampolineTest();
$callback = [$o, 'trampoline'];
$callbackThrow = [$o, 'trampolineThrow'];

$zip = new ZipArchive;
if (!$zip->open($file, ZIPARCHIVE::CREATE)) {
    exit('failed');
}

var_dump($zip->registerCancelCallback($callback));
var_dump($zip->addFromString(PHP_BINARY, 'entry #1'));
var_dump($zip->close());
var_dump($zip->status == ZipArchive::ER_CANCELLED);
var_dump($zip->getStatusString());

echo "Set trampoline after closed Archive:\n";
try {
    var_dump($zip->registerCancelCallback($callback));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

@unlink($file);
unset($zip);

$zip = new ZipArchive;
if (!$zip->open($file, ZIPARCHIVE::CREATE)) {
    exit('failed');
}

var_dump($zip->registerCancelCallback($callbackThrow));
var_dump($zip->addFromString(PHP_BINARY, 'entry #1'));
try {
    var_dump($zip->close());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump($zip->status == ZipArchive::ER_CANCELLED);
var_dump($zip->getStatusString());
@unlink($file);

?>
Done
--CLEAN--
<?php
$dirname = dirname(__FILE__) . '/';
$file = $dirname . '__tmp_oo_cancel_trampoline.zip';

@unlink($file);
?>
--EXPECTF--
bool(true)
bool(true)
Trampoline for trampoline
array(0) {
}

Warning: ZipArchive::close(): Operation cancelled in %s on line %d
bool(false)
bool(true)
string(19) "Operation cancelled"
Set trampoline after closed Archive:
ValueError: Invalid or uninitialized Zip object
bool(true)
bool(true)
Trampoline for trampolineThrow

Warning: ZipArchive::close(): Operation cancelled in %s on line %d
Exception: boo
bool(true)
string(19) "Operation cancelled"
Done
