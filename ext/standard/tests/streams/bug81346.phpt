--TEST--
Bug #81346 (Non-seekable streams don't update position after write)
--DESCRIPTION--
The test expectation is due to bug #81345.
--EXTENSIONS--
bz2
--FILE--
<?php
$s = fopen("compress.bzip2://" . __DIR__ . "/bug81346.bz2", "w");
fwrite($s, str_repeat("hello world", 100));
fflush($s);
var_dump(ftell($s));
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/bug81346.bz2");
?>
--EXPECT--
int(1100)
