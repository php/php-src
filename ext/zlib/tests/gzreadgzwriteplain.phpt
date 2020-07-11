--TEST--
gzopen(), gzread(), gzwrite() for non-compressed data
--SKIPIF--
<?php
if (!extension_loaded("zlib")) print "skip"; ?>
--FILE--
<?php
$original = str_repeat("hallo php",4096);
$filename = tempnam(sys_get_temp_dir(), "phpt");

$fp = fopen($filename, "wb");
fwrite($fp, $original);
var_dump(strlen($original));
var_dump(ftell($fp));
fclose($fp);

$fp = gzopen($filename, "rb");

$data = '';
while ($buf = gzread($fp, 8192)) {
    $data .= $buf;
}

if ($data == $original) {
    echo "Strings are equal\n";
} else {
    echo "Strings are not equal\n";
    var_dump($data);
}

gzseek($fp, strlen($original) / 2);

$data = '';
while ($buf = gzread($fp, 8192)) {
    $data .= $buf;
}

var_dump(strlen($data));
if ($data == substr($original, strlen($original) / 2)) {
    echo "Strings are equal\n";
} else {
    echo "Strings are not equal\n";
    var_dump($data);
}

gzclose($fp);
unlink($filename);
?>
--EXPECT--
int(36864)
int(36864)
Strings are equal
int(18432)
Strings are equal
