--TEST--
Bug #61139 (gzopen leaks when specifying invalid mode)
--EXTENSIONS--
zlib
--FILE--
<?php
try {
    gzopen('someFile', 'c');
} catch (\Throwable $e) {
    echo $e->getMessage() . \PHP_EOL;
}
--CLEAN--
<?php
    unlink('someFile');
?>
--EXPECTF--
gzopen failed
