--TEST--
est file_put_contents() function : usage variation - linked files
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip Do not run on Windows");
?>
--FILE--
<?php
echo "*** Testing file_put_contents() : usage variation ***\n";

$filename = __DIR__.'/filePutContentsVar9.tmp';
$softlink = __DIR__.'/filePutContentsVar9.SoftLink';
$hardlink = __DIR__.'/filePutContentsVar9.HardLink';
$chainlink = __DIR__.'/filePutContentsVar9.ChainLink';


// link files even though it original file doesn't exist yet
symlink($filename, $softlink);
symlink($softlink, $chainlink);


// perform tests
run_test($chainlink);
run_test($softlink);

//can only create a hardlink if the file exists.
file_put_contents($filename,"");
link($filename, $hardlink);
run_test($hardlink);

function run_test($file) {
    $data = "Here is some data";
    $extra = ", more data";
    var_dump(file_put_contents($file, $data));
    var_dump(file_put_contents($file, $extra, FILE_APPEND));
    readfile($file);
    echo "\n";
}


echo "\n*** Done ***\n";
?>
--CLEAN--
<?php
$filename = __DIR__.'/filePutContentsVar9.tmp';
$softlink = __DIR__.'/filePutContentsVar9.SoftLink';
$hardlink = __DIR__.'/filePutContentsVar9.HardLink';
$chainlink = __DIR__.'/filePutContentsVar9.ChainLink';
unlink($chainlink);
unlink($softlink);
unlink($hardlink);
unlink($filename);
?>
--EXPECT--
*** Testing file_put_contents() : usage variation ***
int(17)
int(11)
Here is some data, more data
int(17)
int(11)
Here is some data, more data
int(17)
int(11)
Here is some data, more data

*** Done ***
