--TEST--
sapi_windows_vt100_support(): uncastable user stream
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== "Windows") {
    die("skip Only for Windows systems");
}
?>
--FILE--
<?php

require __DIR__ . '/DummyStreamWrapper.inc';
stream_wrapper_register('custom', DummyStreamWrapper::class);

$fp = fopen("custom://myvar", "r+");
try {
    var_dump(sapi_windows_vt100_support($fp));
} catch (\TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}
fclose($fp);

echo "Done";
?>
--EXPECT--
bool(false)
Done
