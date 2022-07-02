--TEST--
Test fopen() function : variation: use include path create a file (relative)
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
require_once('fopen_include_path.inc');

$thisTestDir =  basename(__FILE__, ".php") . ".dir";
mkdir($thisTestDir);
chdir($thisTestDir);

$newpath = create_include_path();
set_include_path($newpath);
runtest();
$newpath = generate_next_path();
set_include_path($newpath);
runtest();

teardown_include_path();
chdir("..");
rmdir($thisTestDir);

function runtest() {
    global $dir1;
    $tmpfile =  basename(__FILE__, ".php") . ".tmp";
    $h = fopen($tmpfile, "w", true);
    fwrite($h, "This is the test file");
    fclose($h);


    $h = @fopen($tmpfile, "r");
    if ($h === false) {
       echo "Not created in working dir\n";
    }
    else {
       echo "created in working dir\n";
       fclose($h);
       unlink($tmpfile);
    }

    $h = @fopen($dir1.'/'.$tmpfile, "r");
    if ($h === false) {
       echo "Not created in dir1\n";
    }
    else {
       echo "created in dir1\n";
       fclose($h);
       unlink($dir1.'/'.$tmpfile);
    }
}
?>
--EXPECT--
created in working dir
Not created in dir1
created in working dir
Not created in dir1
