--TEST--
.phpc: a pure-PHP file can require a classic .php file
--FILE--
<?php
$dir = __DIR__;
$base = basename(__FILE__, '.php');
$phpc = "$dir/{$base}_main.phpc";
$php  = "$dir/{$base}_lib.php";
file_put_contents($php,  '<?php function greet(string $w): string { return "hi, $w"; }');
file_put_contents($phpc, "require '$php';\necho greet('world'), \"\\n\";");
register_shutdown_function(function () use ($phpc, $php) { @unlink($phpc); @unlink($php); });
require $phpc;
?>
--EXPECT--
hi, world
