--TEST--
.phpc: classic .php behavior is completely unaffected (BC sanity check)
--FILE--
<?php
$file = __DIR__ . '/' . basename(__FILE__, '.php') . '_payload.php';
/* Same payload as .phpc test, but written to a .php file. */
file_put_contents($file, 'echo "hello\n"; $x = 1 + 2; echo $x, "\n";');
register_shutdown_function(fn() => @unlink($file));
require $file;
?>
--EXPECT--
echo "hello\n"; $x = 1 + 2; echo $x, "\n";
