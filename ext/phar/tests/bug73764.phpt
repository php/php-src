--TEST--
Phar: PHP bug #73764: Crash while loading hostile phar archive
--EXTENSIONS--
phar
--FILE--
<?php
chdir(__DIR__);
try {
$p = Phar::LoadPhar('bug73764.phar', 'alias.phar');
echo "OK\n";
} catch(PharException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
PharException: internal corruption of phar "%sbug73764.phar" (truncated manifest entry)
