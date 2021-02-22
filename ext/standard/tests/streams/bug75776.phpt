--TEST--
Bug #75776 (Flushing streams with compression filter is broken)
--SKIPIF--
<?php
if (!extension_loaded('zlib')) die('skip zlib extension not available');
if (!extension_loaded('bz2')) die('skip bz2 extension not available');
?>
--FILE--
<?php
$compression = [
    'gz' => ['zlib.deflate', 'gzinflate'],
    'bz2' => ['bzip2.compress', 'bzdecompress']
];
foreach ($compression as $ext => [$filter, $function]) {
    $stream = fopen(__DIR__ . "/75776.$ext", 'w');
    stream_filter_append($stream, $filter);
    fwrite($stream,"sdfgdfg");
    fflush($stream);
    fclose($stream);

    $compressed = file_get_contents(__DIR__ . "/75776.$ext");
    var_dump($function($compressed));
}
?>
--EXPECT--
string(7) "sdfgdfg"
string(7) "sdfgdfg"
--CLEAN--
<?php
@unlink(__DIR__ . "/75776.gz");
@unlink(__DIR__ . "/75776.bz2");
?>
