--TEST--
stream_isatty(): uncastable user stream
--FILE--
<?php

require __DIR__ . '/DummyStreamWrapper.inc';
stream_wrapper_register('custom', DummyStreamWrapper::class);

$fp = fopen("custom://myvar", "r+");
var_dump(stream_isatty($fp));
fclose($fp);

echo "Done";
?>
--EXPECT--
bool(false)
Done
