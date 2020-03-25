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
internal corruption of phar "%sbug73768.phar" (truncated manifest header)
