--TEST--
.phpc: extension match is strict; ".phpcc" or ".php" with "phpc" in middle are NOT .phpc
--FILE--
<?php
$dir = __DIR__;
$base = basename(__FILE__, '.php');

/* Filename containing "phpc" but not ending in ".phpc" must NOT trigger
 * pure-PHP mode. */
$f1 = "$dir/{$base}_phpc.php";
file_put_contents($f1, 'echo "must-stay-inline";');
register_shutdown_function(fn() => @unlink($f1));
require $f1;
echo "\n";

$f2 = "$dir/{$base}.phpcc";
file_put_contents($f2, 'echo "must-stay-inline-too";');
register_shutdown_function(fn() => @unlink($f2));
require $f2;
?>
--EXPECT--
echo "must-stay-inline";
echo "must-stay-inline-too";
