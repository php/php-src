--TEST--
GH-24170 (bzip2 streams read all concatenated members)
--EXTENSIONS--
bz2
--FILE--
<?php
$file = __DIR__ . '/gh24170.bz2';
$parts = ['', str_repeat('a', 900000), '', 'tail', ''];
$binary = '';
for ($i = 0; $i < 400; $i++) {
    $binary .= hash('sha256', (string) $i, true);
}
$parts[] = $binary;
$parts[] = '';
$compressed = '';
foreach ($parts as $i => $part) {
    $compressed .= bzcompress($part, $i + 1);
}
file_put_contents($file, $compressed);
$expected = implode('', $parts);
var_dump(file_get_contents('compress.bzip2://' . $file) === $expected);
$copy = $file . '.out';
var_dump(copy('compress.bzip2://' . $file, $copy));
var_dump(file_get_contents($copy) === $expected);
unlink($copy);

foreach ([1, 17, 8192, 900000, 1000000] as $size) {
    $fp = fopen('compress.bzip2://' . $file, 'rb');
    $offset = 0;
    $matches = true;
    while (!feof($fp)) {
        $chunk = fread($fp, $size);
        $matches = $matches && $chunk === substr($expected, $offset, strlen($chunk));
        $offset += strlen($chunk);
    }
    var_dump($matches && $offset === strlen($expected));
    var_dump(fread($fp, 1));
    fclose($fp);
}

foreach ([false, true] as $resource) {
    $inner = $resource ? fopen($file, 'rb') : null;
    $fp = bzopen($inner ?? $file, 'r');
    if ($inner) {
        fclose($inner);
    }
    $result = '';
    while (!feof($fp)) {
        $result .= bzread($fp, 8192);
    }
    var_dump($result === $expected);
    var_dump(bzerrno($fp));
    var_dump(bzread($fp, 1));
    bzclose($fp);
}

file_put_contents($file, str_repeat(bzcompress(''), 400) . bzcompress('end'));
var_dump(file_get_contents('compress.bzip2://' . $file));
file_put_contents($file, str_repeat(bzcompress(''), 400));
var_dump(file_get_contents('compress.bzip2://' . $file));
unlink($file);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh24170.bz2');
@unlink(__DIR__ . '/gh24170.bz2.out');
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
string(0) ""
bool(true)
string(0) ""
bool(true)
string(0) ""
bool(true)
string(0) ""
bool(true)
string(0) ""
bool(true)
int(0)
string(0) ""
bool(true)
int(0)
string(0) ""
string(3) "end"
string(0) ""
