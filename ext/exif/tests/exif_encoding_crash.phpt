--TEST--
PHP crash when zend_multibyte_encoding_converter returns (size_t)-1)
--EXTENSIONS--
exif
--FILE--
<?php
$infile = __DIR__.'/exif_encoding_crash.jpg';
$exif_data = exif_read_data($infile);
echo "*** no core dump ***\n";
?>
--EXPECT--
*** no core dump ***
