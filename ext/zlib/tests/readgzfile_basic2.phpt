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
$dirname = 'readgzfile_basic2_temp';
$filename = $dirname.'/readgzfile_basic2.txt';
mkdir($dirname);
$h = fopen($filename, 'w');
fwrite($h, $plaintxt);
fclose($h);


var_dump(readgzfile( $filename ) );
?>
--CLEAN--
<?php
@unlink('readgzfile_basic2_temp/readgzfile_basic2.txt');
@rmdir('readgzfile_basic2_temp');
?>
--EXPECT--
hello world
is a very common test
for all languages
int(52)
