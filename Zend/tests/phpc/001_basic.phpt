--TEST--
.phpc: a file without <?php is parsed as pure PHP
--FILE--
<?php
$file = __DIR__ . '/' . basename(__FILE__, '.php') . '.phpc';
file_put_contents($file, 'echo "hello\n"; $x = 1 + 2; echo $x, "\n";');
register_shutdown_function(fn() => @unlink($file));
require $file;
?>
--EXPECT--
hello
3
