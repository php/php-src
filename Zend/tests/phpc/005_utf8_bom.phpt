--TEST--
.phpc: a leading UTF-8 BOM is silently skipped
--FILE--
<?php
$file = __DIR__ . '/' . basename(__FILE__, '.php') . '.phpc';
file_put_contents($file, "\xEF\xBB\xBF" . 'echo "bom-ok\n";');
register_shutdown_function(fn() => @unlink($file));
require $file;
?>
--EXPECT--
bom-ok
