--TEST--
Testing unlink() function : error conditions
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip.. only on Windows');
}
?>
--FILE--
<?php

$file_path = __DIR__.DIRECTORY_SEPARATOR."unlinkErr私はガラスを食べられます";

@mkdir($file_path);

$filename = "$file_path/unlink_error.tmp";  // temp file name used here
$fp = fopen($filename, "w");  // create file
fclose($fp);

// creating a context
$context = stream_context_create();

echo "*** Testing unlink() : error conditions ***\n";

echo "\n-- Testing unlink() on invalid arguments --\n";
// invalid arguments
var_dump( unlink('') );  // $filename as empty string
var_dump( file_exists('') );  // confirm file doesn't exist

var_dump( unlink(NULL) );  // $filename as NULL
var_dump( file_exists(NULL) );  // confirm file doesn't exist

var_dump( unlink(false) );  // $filename as boolean false
var_dump( file_exists(false) );  // confirm file doesn't exist


echo "\n-- Testing unlink() on non-existent file --\n";
var_dump( unlink(__DIR__."/non_existent_file.tmp") );

echo "\n-- Testing unlink() on directory --\n";
// temp directory used here
$dirname = "$file_path/unlink_error";
// create temp dir
mkdir($dirname);
// unlinking directory
var_dump( unlink($dirname) );  // expected: false as unlink() does not work on dir

echo "Done\n";
?>
--CLEAN--
<?php
unlink(__DIR__."/unlinkErr私はガラスを食べられます/unlink_error.tmp");
rmdir(__DIR__."/unlinkErr私はガラスを食べられます/unlink_error");
rmdir(__DIR__."/unlinkErr私はガラスを食べられます");
?>
--EXPECTF--
*** Testing unlink() : error conditions ***

-- Testing unlink() on invalid arguments --

Warning: unlink(): %s in %s on line %d
bool(false)
bool(false)

Warning: unlink(): %s in %s on line %d
bool(false)
bool(false)

Warning: unlink(): %s in %s on line %d
bool(false)
bool(false)

-- Testing unlink() on non-existent file --

Warning: unlink(%s/non_existent_file.tmp): No such file or directory in %s on line %d
bool(false)

-- Testing unlink() on directory --

Warning: unlink(%s/unlink_error): Is a directory in %s on line %d
bool(false)
Done
