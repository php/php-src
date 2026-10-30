--TEST--
GH-20732 (Phar::LoadPhar undefined behavior when loading directory)
--EXTENSIONS--
phar
--FILE--
<?php
try {
    @Phar::LoadPhar('.');
} catch (PharException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
%r(internal corruption of phar "%s" \(truncated entry\)|unable to open phar for reading ".")%r
