--TEST--
Test gzopen() function : variation: use include path (relative directories in path)
--EXTENSIONS--
zlib
--FILE--
<?php
echo "*** Testing gzopen() : usage variation ***\n";

$testName = 'gzopen_variation4';
require_once('reading_include_path.inc');

//define the files to go into these directories, create one in dir2
echo "\n--- testing include path ---\n";
set_include_path($newIncludePath);
$modes = array("r", "r+", "rt");
foreach($modes as $mode) {
    test_gzopen($mode);
}

// remove the directory structure
chdir($baseDir);
rmdir($workingDir);
foreach($newdirs as $newdir) {
    rmdir($newdir);
}

chdir("..");
rmdir($thisTestDir);

function test_gzopen($mode) {
    global $scriptFile, $secondFile, $firstFile, $filename;

    // create a file in the middle directory
    $handle = gzopen($secondFile, "w");
    gzwrite($handle, "This is a file in dir2");
    gzclose($handle);

    echo "\n** testing with mode=$mode **\n";
    // should read dir2 file
    $handle = null;
    try {
        $handle = gzopen($filename, $mode, true);
    } catch (\Throwable $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    if ($handle) {
        gzpassthru($handle);
        gzclose($handle);
        echo "\n";
    }

    //create a file in dir1
    $handle = gzopen($firstFile, "w");
    gzwrite($handle, "This is a file in dir1");
    gzclose($handle);

    //should now read dir1 file
    $handle = null;
    try {
        $handle = gzopen($filename, $mode, true);
    } catch (\Throwable $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    if ($handle) {
        gzpassthru($handle);
        gzclose($handle);
        echo "\n";
    }

    // create a file in working directory
    $handle = gzopen($filename, "w");
    gzwrite($handle, "This is a file in working dir");
    gzclose($handle);

    //should still read dir1 file
    $handle = null;
    try {
        $handle = gzopen($filename, $mode, true);
    } catch (\Throwable $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    if ($handle) {
        gzpassthru($handle);
        gzclose($handle);
        echo "\n";
    }

    unlink($firstFile);
    unlink($secondFile);

    //should read the file in working dir
    $handle = null;
    try {
        $handle = gzopen($filename, $mode, true);
    } catch (\Throwable $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    if ($handle) {
        gzpassthru($handle);
        gzclose($handle);
        echo "\n";
    }

    // create a file in the script directory
    $handle = gzopen($scriptFile, "w");
    gzwrite($handle, "This is a file in script dir");
    gzclose($handle);

    //should read the file in script dir
    $handle = null;
    try {
        $handle = gzopen($filename, $mode, true);
    } catch (\Throwable $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    if ($handle) {
        gzpassthru($handle);
        gzclose($handle);
        echo "\n";
    }

    //cleanup
    unlink($filename);
    unlink($scriptFile);
}

?>
--EXPECTF--
*** Testing gzopen() : usage variation ***

--- testing include path ---

** testing with mode=r **
This is a file in dir2
This is a file in dir1
This is a file in dir1
This is a file in working dir
This is a file in script dir

** testing with mode=r+ **
Cannot open a zlib stream for reading and writing at the same time!
Cannot open a zlib stream for reading and writing at the same time!
Cannot open a zlib stream for reading and writing at the same time!
Cannot open a zlib stream for reading and writing at the same time!
Cannot open a zlib stream for reading and writing at the same time!

** testing with mode=rt **
This is a file in dir2
This is a file in dir1
This is a file in dir1
This is a file in working dir
This is a file in script dir
