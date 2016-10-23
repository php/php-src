--TEST--
Test for bug #65701: copy() doesn't work when destination filename is created by tempnam()
--CREDITS--
Boro Sitnikovski <buritomath@yahoo.com>
--FILE--
<?php
$file_path = dirname(__FILE__) . "/bug65701/";

if (!is_dir($file_path)) {
	mkdir($file_path);
}

$src = $file_path . '/srcbug65701_file.txt';
$dst = tempnam($file_path, 'dstbug65701_file.txt');

file_put_contents($src, "Hello World");

copy($src, $dst);
var_dump(filesize($dst));
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__) . "/bug65701/";
foreach (scandir($file_path) as $file) {
    if (strpos($file, "bug65701") !== false || 'WIN' == substr(PHP_OS, 0, 3)) {
        unlink($file_path . $file);
    }
}
rmdir($file_path);
?>
--EXPECT--
int(11)
