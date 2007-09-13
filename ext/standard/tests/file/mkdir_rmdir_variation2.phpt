--TEST--
Test mkdir() and rmdir() functions: usage variations - misc.
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. only on LINUX');
}
// Skip if being run by root (files are always readable, writeable and executable)
$filename = dirname(__FILE__)."/is_readable_root_check.tmp";
$fp = fopen($filename, 'w');
fclose($fp);
if(fileowner($filename) == 0) {
	unlink ($filename);
	die('skip cannot be run as root');
}

unlink($filename);
?>
--FILE--
<?php
/*  Prototype: bool mkdir ( string $pathname [, int $mode [, bool $recursive [, resource $context]]] );
    Description: Makes directory
*/

$context = stream_context_create();

$file_path = dirname(__FILE__);

echo "\n*** Testing mkdir() and rmdir() by giving stream context as fourth argument ***\n";
var_dump( mkdir("$file_path/mkdir_variation2/test/", 0777, true, $context) );
var_dump( rmdir("$file_path/mkdir_variation2/test/", $context) );

echo "\n*** Testing rmdir() on a non-empty directory ***\n";
var_dump( mkdir("$file_path/mkdir_variation2/test/", 0777, true) );
var_dump( rmdir("$file_path/mkdir_variation2/") );

echo "\n*** Testing mkdir() and rmdir() for binary safe functionality ***\n";
var_dump( mkdir("$file_path/temp".chr(0)."/") );
var_dump( rmdir("$file_path/temp".chr(0)."/") );

echo "\n*** Testing mkdir() with miscelleneous input ***\n";
/* changing mode of mkdir to prevent creating sub-directory under it */
var_dump( chmod("$file_path/mkdir_variation2/", 0000) );
/* creating sub-directory test1 under mkdir, expected: false */
var_dump( mkdir("$file_path/mkdir_variation2/test1", 0777, true) );
var_dump( chmod("$file_path/mkdir_variation2/", 0777) );  // chmod to enable removing test1 directory

echo "Done\n";
?>
--CLEAN--
<?php
rmdir(dirname(__FILE__)."/mkdir_variation2/test/");
rmdir(dirname(__FILE__)."/mkdir_variation2/test1/");
rmdir(dirname(__FILE__)."/mkdir_variation2/");
?>
--EXPECTF--
*** Testing mkdir() and rmdir() by giving stream context as fourth argument ***
bool(true)
bool(true)

*** Testing rmdir() on a non-empty directory ***
bool(true)

Warning: rmdir(%s/mkdir_variation2/): %s on line %d
bool(false)

*** Testing mkdir() and rmdir() for binary safe functionality ***
bool(true)
bool(true)

*** Testing mkdir() with miscelleneous input ***
bool(true)

Warning: mkdir(): Permission denied in %s on line %d
bool(false)
bool(true)
Done
