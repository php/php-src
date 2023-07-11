--TEST--
posix_ttyname(): uncastable user stream
--EXTENSIONS--
posix
--FILE--
<?php

require dirname(__DIR__, 3) . '/tests/output/DummyStreamWrapper.inc';
stream_wrapper_register('custom', DummyStreamWrapper::class);

$fp = fopen("custom://myvar", "r+");
var_dump(posix_ttyname($fp));
fclose($fp);

echo "Done";
?>
--EXPECTF--
Warning: posix_ttyname(): Could not use stream of type 'user-space' in %s on line %d
bool(false)
Done
