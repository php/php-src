--TEST--
Zend Multibyte does not read past the script during UTF-16 detection
--EXTENSIONS--
mbstring
--INI--
zend.multibyte=1
internal_encoding=UTF-8
--FILE--
<?php
$filename = __DIR__ . '/multibyte_encoding_008.tmp.php';
file_put_contents($filename, "<\0?\0p\0h\0p\0");
include $filename;
echo "Done\n";
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/multibyte_encoding_008.tmp.php');
?>
--EXPECT--
Done
