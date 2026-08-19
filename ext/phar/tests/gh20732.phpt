--TEST--
GH-20732 (Phar::loadPhar undefined behavior when loading directory)
--EXTENSIONS--
phar
--FILE--
<?php
try {
    @Phar::loadPhar('.');
} catch (PharException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
%r(internal corruption of phar "%s" \(truncated entry\)|unable to open phar for reading ".")%r
