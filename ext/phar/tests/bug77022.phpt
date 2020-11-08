--TEST--
Phar: Bug #77022: PharData always creates new files with mode 0666
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
if (defined("PHP_WINDOWS_VERSION_MAJOR")) die("skip not for Windows")
?>
--FILE--
<?php
umask(022);
var_dump(decoct(umask()));

$sFile = tempnam(__DIR__, 'test77022');
var_dump(decoct(stat($sFile)['mode']));

foreach([Phar::TAR => 'tar', Phar::ZIP => 'zip'] as $mode => $ext) {
    clearstatcache();
    $phar = new PharData(__DIR__ . '/test77022.' . $ext, null, null, $mode);
    $phar->addFile($sFile, 'test-file-phar');
    $phar->addFromString("test-from-string", 'test-file-phar');
    $phar->extractTo(__DIR__);
    var_dump(decoct(stat(__DIR__ . '/test-file-phar')['mode']));
    var_dump(decoct(stat(__DIR__ . '/test-from-string')['mode']));
    unlink(__DIR__ . '/test-file-phar');
    unlink(__DIR__ . '/test-from-string');
    unlink(__DIR__ . '/test77022.' . $ext);
}
unlink($sFile);
?>
--EXPECT--
string(2) "22"
string(6) "100600"
string(6) "100600"
string(6) "100644"
string(6) "100600"
string(6) "100644"