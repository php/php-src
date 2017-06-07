--TEST--
Bug #70417 (PharData::compress() doesn't close temp file)
--SKIPIF--
<?php
if (!extension_loaded('phar') || !extension_loaded('zlib')) {
    die("skip ext/phar or ext/zlib not available");
}
exec('lsof -p ' . getmypid(), $out, $status);
if ($status !== 0) {
    die("skip lsof(8) not available");
}
?>
--FILE--
<?php
function countOpenFiles() {
    exec('lsof -p ' . getmypid(), $out);
    return count($out);
}
$filename = __DIR__ . '/bug70417.tar';
@unlink("$filename.gz");
$openFiles1 = countOpenFiles();
$arch = new PharData($filename);
$arch->addFromString('foo', 'bar');
$arch->compress(Phar::GZ);
unset($arch);
$openFiles2 = countOpenFiles();
var_dump($openFiles1 === $openFiles2);
?>
--CLEAN--
<?php
$filename = __DIR__ . '/bug70417.tar';
@unlink($filename);
@unlink("$filename.gz");
?>
--EXPECT--
bool(true)
