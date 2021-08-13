--TEST--
Phar: Bug #79082: Files added to tar with Phar::buildFromIterator have all-access permissions
--EXTENSIONS--
phar
--SKIPIF--
<?php
if (defined("PHP_WINDOWS_VERSION_MAJOR")) die("skip not for Windows")
?>
--FILE--
<?php
umask(022);
var_dump(decoct(umask()));
chmod(__DIR__ . '/test79082/test79082-testfile', 0644);
chmod(__DIR__ . '/test79082/test79082-testfile2', 0400);

foreach([Phar::TAR => 'tar', Phar::ZIP => 'zip'] as $format => $ext) {
    clearstatcache();
    $phar = new PharData(__DIR__ . '/test79082.' . $ext, format: $format);
    $phar->buildFromIterator(new \RecursiveDirectoryIterator(__DIR__ . '/test79082', \FilesystemIterator::SKIP_DOTS), __DIR__ . '/test79082');
    $phar->extractTo(__DIR__);
    var_dump(decoct(stat(__DIR__ . '/test79082-testfile')['mode']));
    var_dump(decoct(stat(__DIR__ . '/test79082-testfile2')['mode']));
    unlink(__DIR__ . '/test79082-testfile');
    unlink(__DIR__ . '/test79082-testfile2');
}
foreach([Phar::TAR => 'tar', Phar::ZIP => 'zip'] as $format => $ext) {
    clearstatcache();
    $phar = new PharData(__DIR__ . '/test79082-d.' . $ext, format: $format);
    $phar->buildFromDirectory(__DIR__ . '/test79082');
    $phar->extractTo(__DIR__);
    var_dump(decoct(stat(__DIR__ . '/test79082-testfile')['mode']));
    var_dump(decoct(stat(__DIR__ . '/test79082-testfile2')['mode']));
    unlink(__DIR__ . '/test79082-testfile');
    unlink(__DIR__ . '/test79082-testfile2');
}
?>
--CLEAN--
<?php
unlink(__DIR__ . '/test79082.tar');
unlink(__DIR__ . '/test79082.zip');
unlink(__DIR__ . '/test79082-d.tar');
unlink(__DIR__ . '/test79082-d.zip');
?>
--EXPECT--
string(2) "22"
string(6) "100644"
string(6) "100400"
string(6) "100644"
string(6) "100400"
string(6) "100644"
string(6) "100400"
string(6) "100644"
string(6) "100400"
