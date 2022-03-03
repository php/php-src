--TEST--
Test gzopen() function : variation: relative/absolute file
--EXTENSIONS--
zlib
--FILE--
<?php
echo "*** Testing gzopen() : variation ***\n";
$absfile = __FILE__.'.tmp';
$relfile = "gzopen_variation6.tmp";

$h = gzopen($absfile, "w");
gzwrite($h, "This is an absolute file");
gzclose($h);

$h = gzopen($relfile, "w");
gzwrite($h, "This is a relative file");
gzclose($h);

$h = gzopen($absfile, "r");
gzpassthru($h);
fclose($h);
echo "\n";

$h = gzopen($relfile, "r");
gzpassthru($h);
gzclose($h);
echo "\n";

unlink($absfile);
unlink($relfile);
?>
--EXPECT--
*** Testing gzopen() : variation ***
This is an absolute file
This is a relative file
