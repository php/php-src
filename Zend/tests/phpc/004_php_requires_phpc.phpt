--TEST--
.phpc: a classic .php file can require a .phpc file
--FILE--
<?php
$dir = __DIR__;
$base = basename(__FILE__, '.php');
$phpc = "$dir/{$base}_lib.phpc";
file_put_contents($phpc, "function add(int \$a, int \$b): int { return \$a + \$b; }");
register_shutdown_function(fn() => @unlink($phpc));
require $phpc;
echo add(2, 3), "\n";
?>
--EXPECT--
5
