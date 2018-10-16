--TEST--
Phar: PHP bug #73764: Crash while loading hostile phar archive
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
chdir(__DIR__);
try {
$p = Phar::LoadPhar('bug73764.phar', 'alias.phar');
echo "OK\n";
} catch(PharException $e) {
	echo $e->getMessage();
}
?>
--EXPECTF--
internal corruption of phar "%sbug73764.phar" (truncated manifest entry)
