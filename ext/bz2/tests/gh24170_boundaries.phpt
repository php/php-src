--TEST--
GH-24170 (concatenated bzip2 headers split across the compressed input buffer)
--EXTENSIONS--
bz2
--FILE--
<?php
$file = __DIR__ . '/gh24170_boundaries.bz2';
foreach ([[0, 4502], [2, 4483], [0, 4521], [2, 4479]] as [$seed, $length]) {
    $data = '';
    for ($i = 0; $i < 200; $i++) {
        $data .= hash('sha256', "$seed:$i", true);
    }
    $data = substr($data, 0, $length);
    $compressed = bzcompress($data, 9);
    var_dump(strlen($compressed));
    file_put_contents($file, $compressed . bzcompress('tail'));
    $fp = fopen('compress.bzip2://file://' . $file, 'rb');
    stream_set_chunk_size($fp, 1);
    var_dump(fread($fp, 10000) === $data . 'tail');
    var_dump(feof($fp));
    fclose($fp);
}
unlink($file);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh24170_boundaries.bz2');
?>
--EXPECT--
int(4997)
bool(true)
bool(true)
int(4998)
bool(true)
bool(true)
int(4999)
bool(true)
bool(true)
int(5000)
bool(true)
bool(true)
