--TEST--
Test opendir() function : usage variations - different directory permissions
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) == 'WIN') {
  die('skip Not for Windows');
}
// Skip if being run by root (files are always readable, writeable and executable)
$filename = dirname(__FILE__) . "/opendir_root_check.tmp";
$fp = fopen($filename, 'w');
fclose($fp);
if(fileowner($filename) == 0) {
        unlink ($filename);
        die('skip...cannot be run as root\n');
}
unlink($filename);
?>
--FILE--
<?php
/* Prototype  : mixed opendir(string $path[, resource $context])
 * Description: Open a directory and return a dir_handle 
 * Source code: ext/standard/dir.c
 */

/*
 * Open a directory using opendir() with different directory permissions
 */

echo "*** Testing opendir() : usage variations ***\n";

// create the temporary directory
$file_path = dirname(__FILE__);
$dir_path = $file_path . "/opendir_variation7";
mkdir($dir_path);

/* different values for directory permissions */
$permission_values = array(
/*1*/  0477,  // owner has read only, other and group has rwx
       0677,  // owner has rw only, other and group has rwx

/*3*/  0444,  // all have read only
       0666,  // all have rw only

/*5*/  0400,  // owner has read only, group and others have no permission
       0600,   // owner has rw only, group and others have no permission

/*7*/  0470,  // owner has read only, group has rwx & others have no permission
       0407,  // owner has read only, other has rwx & group has no permission

/*9*/  0670,  // owner has rw only, group has rwx & others have no permission
/*10*/ 0607   // owner has rw only, group has no permission and others have rwx
);

// Open directory with different permission values, read and close, expected: none of them to succeed.

$iterator = 1;
foreach ($permission_values as $perm) {

	echo "\n-- Iteration $iterator --\n";
	// try to remove the dir if exists  & create
	if (is_dir($dir_path)){
		chmod ($dir_path, 0777); // change dir permission to allow all operation
		rmdir ($dir_path);
	}
	mkdir($dir_path);

	// change the dir permisson to test dir on it
	var_dump( chmod($dir_path, $perm) );

	var_dump($dh = opendir($dir_path));
	
	if (is_resource($dh)) {
		closedir($dh);
	}
	$iterator++;
}
?>
===DONE===
--CLEAN--
<?php
// deleting temporary directory
$dir_path = dirname(__FILE__) . "/opendir_variation7";
rmdir($dir_path);
?>
--EXPECTF--
*** Testing opendir() : usage variations ***

-- Iteration 1 --
bool(true)
resource(%d) of type (stream)

-- Iteration 2 --
bool(true)
resource(%d) of type (stream)

-- Iteration 3 --
bool(true)
resource(%d) of type (stream)

-- Iteration 4 --
bool(true)
resource(%d) of type (stream)

-- Iteration 5 --
bool(true)
resource(%d) of type (stream)

-- Iteration 6 --
bool(true)
resource(%d) of type (stream)

-- Iteration 7 --
bool(true)
resource(%d) of type (stream)

-- Iteration 8 --
bool(true)
resource(%d) of type (stream)

-- Iteration 9 --
bool(true)
resource(%d) of type (stream)

-- Iteration 10 --
bool(true)
resource(%d) of type (stream)
===DONE===
