--TEST--
Test tempnam() function: usage variations - provide optional suffix
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    die("skip Do not run on Windows");
}
?>
--FILE--
<?php
/* Passing the optional $suffix arg */

echo "*** Testing tempnam() with suffixes ***\n";
$file_path = __DIR__."/tempnamVar10";
mkdir($file_path);
$prefix = "prefix-";

/* An array of suffixes */
$names_arr = array(
    "",
    "suffix",
    "-suffix",
    ".ext",
    /* suffix with path separators should get basename to be consistent
     * with the prefix behavior */
    "/no/such/file/dir",
    "php/php",
    NULL,
    1,
    0,
    TRUE,
    FALSE,
    array(),
    " ",
    "\0",
);

foreach($names_arr as $i=>$suffix) {
    echo "-- Iteration $i --\n";
    try {
        $file_name = tempnam("$file_path", $prefix, $suffix);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
        continue;
    }

    /* creating the files in existing dir */
    if (file_exists($file_name)) {
        echo "File name is => ";
        print($file_name);
        echo "\n";

        echo "File permissions are => ";
        printf("%o", fileperms($file_name) );
        echo "\n";

        echo "File created in => ";
        $file_dir = dirname($file_name);

        if ($file_dir == sys_get_temp_dir()) {
            echo "temp dir\n";
        }
        else if ($file_dir == $file_path) {
            echo "directory specified\n";
        }
        else {
            echo "unknown location\n";
        }

    }
    else {
        echo "-- File is not created --\n";
    }

    unlink($file_name);
}

rmdir($file_path);
?>
--CLEAN--
<?php
$file_path = __DIR__."/tempnamVar10";
if (file_exists($file_path)) {
    array_map('unlink', glob($file_path . "/*"));
    rmdir($file_path);
}
?>
--EXPECTF--
*** Testing tempnam() with suffixes ***
-- Iteration 0 --
File name is => %s%eprefix-%r.{6}%r
File permissions are => 100600
File created in => directory specified
-- Iteration 1 --
File name is => %s%eprefix-%r.{6}%rsuffix
File permissions are => 100600
File created in => directory specified
-- Iteration 2 --
File name is => %s%eprefix-%r.{6}%r-suffix
File permissions are => 100600
File created in => directory specified
-- Iteration 3 --
File name is => %s%eprefix-%r.{6}%r.ext
File permissions are => 100600
File created in => directory specified
-- Iteration 4 --
File name is => %s%eprefix-%r.{6}%rdir
File permissions are => 100600
File created in => directory specified
-- Iteration 5 --
File name is => %s%eprefix-%r.{6}%rphp
File permissions are => 100600
File created in => directory specified
-- Iteration 6 --

Deprecated: tempnam(): Passing null to parameter #3 ($suffix) of type string is deprecated in %s on line %d
File name is => %s%eprefix-%r.{6}%r
File permissions are => 100600
File created in => directory specified
-- Iteration 7 --
File name is => %s%eprefix-%r.{6}%r1
File permissions are => 100600
File created in => directory specified
-- Iteration 8 --
File name is => %s%eprefix-%r.{6}%r0
File permissions are => 100600
File created in => directory specified
-- Iteration 9 --
File name is => %s%eprefix-%r.{6}%r1
File permissions are => 100600
File created in => directory specified
-- Iteration 10 --
File name is => %s%eprefix-%r.{6}%r
File permissions are => 100600
File created in => directory specified
-- Iteration 11 --
tempnam(): Argument #3 ($suffix) must be of type string, array given
-- Iteration 12 --
File name is => %s%eprefix-%r.{6}%r 
File permissions are => 100600
File created in => directory specified
-- Iteration 13 --
tempnam(): Argument #3 ($suffix) must not contain any null bytes
