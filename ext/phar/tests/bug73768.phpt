--TEST--
Phar: PHP bug #73768: Memory corruption when loading hostile phar
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
chdir(__DIR__);
try {
$p = Phar::LoadPhar('bug73768.phar', 'alias.phar');
echo "OK\n";
} catch(PharException $e) {
	echo $e->getMessage();
}
?>
--EXPECTF--
cannot load phar "%sbug73768.phar" with implicit alias "" under different alias "alias.phar"
