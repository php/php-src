--TEST--
Test tempnam() function: usage variations - obscure prefixes
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != "WIN")
	die("skip run only on Windows");
?>
--CONFLICTS--
obscure_filename
--FILE--
<?php
/* Prototype:  string tempnam ( string $dir, string $prefix );
   Description: Create file with unique file name.
*/

/* Passing invalid/non-existing args for $prefix */

echo "*** Testing tempnam() with obscure prefixes ***\n";
$file_path = __DIR__."/tempnamVar3";
if (!mkdir($file_path)) {
	echo "Failed, cannot create temp dir $filepath\n";
	exit(1);
}

$file_path = realpath($file_path);

/* An array of prefixes */
$names_arr = array(
	/* Valid args (casting)*/
	-1,
	TRUE,
	FALSE,
	NULL,
	"",
	" ",
	"\0",
	/* Invalid args */
	array(),

	/* Valid args*/
	/* prefix with path separator of a non existing directory*/
	"/no/such/file/dir",
	"php/php"
);

$res_arr = array(
	/* Invalid args */
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	false,

	/* prefix with path separator of a non existing directory*/
	true,
	true
);

for( $i=0; $i<count($names_arr); $i++ ) {
	echo "-- Iteration $i --\n";
	$file_name = tempnam($file_path, $names_arr[$i]);

	/* creating the files in existing dir */
	if (file_exists($file_name) && !$res_arr[$i]) {
		echo "Failed\n";
	}
	if ($res_arr[$i]) {
		$file_dir = dirname($file_name);
		if (realpath($file_dir) == $file_path || realpath($file_dir . "\\") == $file_path) {
			echo "OK\n";
		} else {
			echo "Failed, not created in the correct directory " . realpath($file_dir) . ' vs ' . $file_path ."\n";
		}

		if (!is_writable($file_name)) {
			printf("%o\n", fileperms($file_name) );

		}
	} else {
		echo "OK\n";
	}
	@unlink($file_name);
}

rmdir($file_path);
?>
--EXPECTF--
*** Testing tempnam() with obscure prefixes ***
-- Iteration 0 --
OK
-- Iteration 1 --
OK
-- Iteration 2 --
OK
-- Iteration 3 --
OK
-- Iteration 4 --
OK
-- Iteration 5 --

Notice: tempnam(): file created in the system's temporary directory in %stempnam_variation3-win32.php on line %d
Failed, not created in the correct directory %s vs %s
0
-- Iteration 6 --

Warning: tempnam() expects parameter 2 to be a valid path, string given in %stempnam_variation3-win32.php on line 54
Failed, not created in the correct directory %s vs %sext\standard\tests\file\tempnamVar3
0
-- Iteration 7 --

Warning: tempnam() expects parameter 2 to be a valid path, array given in %s\ext\standard\tests\file\tempnam_variation3-win32.php on line %d
OK
-- Iteration 8 --
OK
-- Iteration 9 --
OK
