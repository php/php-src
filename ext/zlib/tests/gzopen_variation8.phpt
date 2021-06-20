--TEST--
Test gzopen() function : variation: opening a plain file
--EXTENSIONS--
zlib
--FILE--
<?php
echo "*** Testing gzopen() : variation ***\n";

$data = <<<EOT
Here is some plain
text to be read
and displayed.
EOT;

$file = "gzopen_variation8.tmp";
$h = fopen($file, 'w');
fwrite($h, $data);
fclose($h);

$h = gzopen($file, 'r');
gzpassthru($h);
gzclose($h);
echo "\n";
unlink($file);
?>
--EXPECT--
*** Testing gzopen() : variation ***
Here is some plain
text to be read
and displayed.
