--TEST--
Test function readgzfile() reading a plain relative file
--EXTENSIONS--
zlib
--FILE--
<?php
$plaintxt = <<<EOT
hello world
is a very common test
for all languages

EOT;
$dirname = 'readgzfile_temp';
$filename = $dirname.'/readgzfile_basic2.txt';
mkdir($dirname);
$h = fopen($filename, 'w');
fwrite($h, $plaintxt);
fclose($h);


var_dump(readgzfile( $filename ) );

unlink($filename);
rmdir($dirname);
?>
--EXPECT--
hello world
is a very common test
for all languages
int(52)
