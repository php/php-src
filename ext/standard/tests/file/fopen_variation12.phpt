--TEST--
Test fopen() function : variation: use include path (path is bad) create a file (relative)
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
set_include_path("rubbish");
testme();


function testme() {
    $tmpfile = basename(__FILE__, ".php") . ".tmp";
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


    $scriptDirFile = __DIR__.'/'.$tmpfile;
    $h = @fopen($scriptDirFile, "r");
    if ($h === false) {
       echo "Not created in script dir\n";
    }
    else {
       echo "created in script dir\n";
       fclose($h);
       unlink($scriptDirFile);
    }
}
?>
--EXPECT--
created in working dir
Not created in script dir
