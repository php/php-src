--TEST--
GH-20732 (Phar::LoadPhar undefined behavior when loading directory)
--EXTENSIONS--
phar
--FILE--
<?php
try {
    Phar::LoadPhar('.');
} catch (PharException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Notice: Phar::loadPhar(): Read of 8192 bytes failed with errno=21 Is a directory in %s on line %d
internal corruption of phar "/run/media/niels/MoreData/php-8.3" (truncated entry)
