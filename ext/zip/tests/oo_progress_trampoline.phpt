--TEST--
ZipArchive::registerProgressCallback() with a trampoline
--EXTENSIONS--
zip
--SKIPIF--
<?php
/* $Id$ */
if (!method_exists('ZipArchive', 'registerProgressCallback')) die('skip libzip too old');
?>
--INI--
date.timezone=UTC
--FILE--
<?php
$dirname = dirname(__FILE__) . '/';
$file = $dirname . '__tmp_oo_progress_trampoline.zip';

class TrampolineTest {
    public function __call(string $name, array $arguments): void {
        echo 'Trampoline for ', $name, PHP_EOL;
        if ($name === 'trampolineThrow') {
            throw new Exception('boo');
        }
        var_dump($arguments);
        $r = $arguments[0];
        if ($r == 0.0) echo "start\n";
        if ($r == 1.0) echo "end\n";
    }
}
$o = new TrampolineTest();
$callback = [$o, 'trampoline'];
$callbackThrow = [$o, 'trampolineThrow'];


$zip = new ZipArchive;
if (!$zip->open($file, ZIPARCHIVE::CREATE)) {
    exit('failed');
}
var_dump($zip->registerProgressCallback(0.5, $callback));
var_dump($zip->addFromString('foo', 'entry #1'));
var_dump($zip->close());

echo "Set trampoline after closed Archive:\n";
try {
    var_dump($zip->registerProgressCallback(0.5, $callback));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

unlink($file);
unset($zip);

$zip = new ZipArchive;
if (!$zip->open($file, ZIPARCHIVE::CREATE)) {
    exit('failed');
}

var_dump($zip->registerProgressCallback(0.5, $callbackThrow));
var_dump($zip->addFromString('foo', 'entry #1'));
try {
    var_dump($zip->close());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
unlink($file);
unset($zip);
?>
Done
--CLEAN--
<?php
$dirname = dirname(__FILE__) . '/';
$file = $dirname . '__tmp_oo_progress_trampoline.zip';

@unlink($file);
?>
--EXPECT--
bool(true)
bool(true)
Trampoline for trampoline
array(1) {
  [0]=>
  float(0)
}
start
Trampoline for trampoline
array(1) {
  [0]=>
  float(1)
}
end
bool(true)
Set trampoline after closed Archive:
ValueError: Invalid or uninitialized Zip object
bool(true)
bool(true)
Trampoline for trampolineThrow
Exception: boo
Done
