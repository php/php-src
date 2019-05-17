--TEST--
Test closedir() function : usage variations - close a file pointer
--FILE--
<?php
/* Prototype  : void closedir([resource $dir_handle])
 * Description: Close directory connection identified by the dir_handle
 * Source code: ext/standard/dir.c
 * Alias to functions: close
 */

/*
 * Create a file pointer using fopen() then try to close it using closedir()
 */

echo "*** Testing closedir() : usage variations ***\n";

echo "\n-- Open a file using fopen() --\n";
var_dump($fp = fopen(__FILE__, 'r'));

echo "\n-- Try to close the file pointer using closedir() --\n";
var_dump(closedir($fp));

echo "\n-- Check file pointer: --\n";
var_dump($fp);

if(is_resource($fp)) {
	fclose($fp);
}
?>
===DONE===
--EXPECTF--
*** Testing closedir() : usage variations ***

-- Open a file using fopen() --
resource(%d) of type (stream)

-- Try to close the file pointer using closedir() --

Warning: closedir(): %d is not a valid Directory resource in %s on line %d
bool(false)

-- Check file pointer: --
resource(%d) of type (stream)
===DONE===
