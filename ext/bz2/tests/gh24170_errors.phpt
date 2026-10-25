--TEST--
GH-24170 (concatenated bzip2 streams handle trailing data and damaged members)
--EXTENSIONS--
bz2
--FILE--
<?php
$file = __DIR__ . '/gh24170_errors.bz2';
foreach (['', 'B', 'BZ', 'BZh', 'BZh0', 'BZh:', 'garbage'] as $suffix) {
    file_put_contents($file, bzcompress('data') . $suffix);
    var_dump(file_get_contents('compress.bzip2://' . $file));
}

$damaged = bzcompress('broken');
$damaged[10] = ~$damaged[10];
foreach (['BZh9', substr(bzcompress('broken'), 0, -1), $damaged] as $suffix) {
    file_put_contents($file, bzcompress('data') . $suffix);
    $fp = bzopen($file, 'r');
    var_dump(bzread($fp, 4));
    var_dump(bzread($fp, 8192));
    var_dump(bzerrno($fp) < 0);
    var_dump(feof($fp));
    var_dump(bzread($fp, 8192));
    bzclose($fp);
}
unlink($file);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh24170_errors.bz2');
?>
--EXPECT--
string(4) "data"
string(4) "data"
string(4) "data"
string(4) "data"
string(4) "data"
string(4) "data"
string(4) "data"
string(4) "data"
bool(false)
bool(true)
bool(true)
bool(false)
string(4) "data"
bool(false)
bool(true)
bool(true)
bool(false)
string(4) "data"
bool(false)
bool(true)
bool(true)
bool(false)
