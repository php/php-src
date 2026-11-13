--TEST--
.phpc: an empty file produces no output and no error
--FILE--
<?php
$file = __DIR__ . '/' . basename(__FILE__, '.php') . '.phpc';
file_put_contents($file, '');
register_shutdown_function(fn() => @unlink($file));
require $file;
echo "after-require\n";
?>
--EXPECT--
after-require
