--TEST--
gzopen(), gzread(), gzwrite()
--EXTENSIONS--
zlib
--FILE--
<?php
$original = str_repeat("hallo php",4096);
$filename = tempnam(sys_get_temp_dir(), "phpt");

$fp = gzopen($filename, "wb");
gzwrite($fp, $original);
var_dump(strlen($original));
var_dump(gztell($fp));
fclose($fp);

$fp = gzopen($filename, "rb");

$data = '';
while ($buf = gzread($fp, 8092)) {
    $data .= $buf;
}

if ($data == $original) {
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
