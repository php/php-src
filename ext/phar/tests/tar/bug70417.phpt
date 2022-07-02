--TEST--
Bug #70417 (PharData::compress() doesn't close temp file)
--EXTENSIONS--
phar
zlib
--SKIPIF--
<?php

exec('lsof -p ' . getmypid(), $out, $status);
if ($status !== 0) {
    die("skip lsof(8) not available");
}
if (!str_starts_with($out[0], 'COMMAND')) {
    die("skip Might be a different lsof");
}
?>
--FILE--
<?php
function countOpenFiles() {
    exec('lsof -p ' . escapeshellarg(getmypid()) . ' 2> /dev/null', $out);  // Note: valgrind can produce false positives for /usr/bin/lsof
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
