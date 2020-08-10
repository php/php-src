--TEST--
Bug #67761 (Phar::mapPhar fails for Phars inside a path containing ".tar")
--SKIPIF--
<?php extension_loaded("phar") or die("SKIP need ext/phar support"); ?>
--FILE--
<?php

echo "Test\n";

include __DIR__."/files/bug67761.tar/bug67761.phar";

?>
--EXPECT--
Test
Test
