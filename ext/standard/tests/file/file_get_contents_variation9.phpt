--TEST--
Test file_get_contents() function : variation - linked files
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip Do not run on Windows");
?>
--FILE--
<?php
echo "*** Testing file_get_contents() : variation ***\n";
$filename = __DIR__.'/fileGetContentsVar9.tmp';
$softlink = __DIR__.'/fileGetContentsVar9.SoftLink';
$hardlink = __DIR__.'/fileGetContentsVar9.HardLink';
$chainlink = __DIR__.'/fileGetContentsVar9.ChainLink';

// create file
$h = fopen($filename,"w");
//Data should be more than the size of a link.
for ($i = 1; $i <= 10; $i++) {
   fwrite($h, "Here is a repeated amount of data");
}
fclose($h);

// link files
link($filename, $hardlink);
symlink($filename, $softlink);
symlink($softlink, $chainlink);

// perform tests
var_dump(file_get_contents($chainlink));
var_dump(file_get_contents($softlink));
var_dump(file_get_contents($hardlink));

echo "\n*** Done ***\n";
?>
--CLEAN--
<?php
$filename = __DIR__.'/fileGetContentsVar9.tmp';
$softlink = __DIR__.'/fileGetContentsVar9.SoftLink';
$hardlink = __DIR__.'/fileGetContentsVar9.HardLink';
$chainlink = __DIR__.'/fileGetContentsVar9.ChainLink';
unlink($chainlink);
unlink($softlink);
unlink($hardlink);
unlink($filename);
?>
--EXPECT--
*** Testing file_get_contents() : variation ***
string(330) "Here is a repeated amount of dataHere is a repeated amount of dataHere is a repeated amount of dataHere is a repeated amount of dataHere is a repeated amount of dataHere is a repeated amount of dataHere is a repeated amount of dataHere is a repeated amount of dataHere is a repeated amount of dataHere is a repeated amount of data"
string(330) "Here is a repeated amount of dataHere is a repeated amount of dataHere is a repeated amount of dataHere is a repeated amount of dataHere is a repeated amount of dataHere is a repeated amount of dataHere is a repeated amount of dataHere is a repeated amount of dataHere is a repeated amount of dataHere is a repeated amount of data"
string(330) "Here is a repeated amount of dataHere is a repeated amount of dataHere is a repeated amount of dataHere is a repeated amount of dataHere is a repeated amount of dataHere is a repeated amount of dataHere is a repeated amount of dataHere is a repeated amount of dataHere is a repeated amount of dataHere is a repeated amount of data"

*** Done ***
