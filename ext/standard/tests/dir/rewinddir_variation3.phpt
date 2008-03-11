--TEST--
Test rewinddir() function : usage variations - file pointers
--FILE--
<?php
/* Prototype  : void rewinddir([resource $dir_handle])
 * Description: Rewind dir_handle back to the start 
 * Source code: ext/standard/dir.c
 * Alias to functions: rewind
 */

/*
 * Pass a file pointer to rewinddir() to test behaviour
 */

echo "*** Testing rewinddir() : usage variations ***\n";

echo "\n-- Open a file using fopen --\n";
var_dump($fp = fopen(__FILE__, 'r'));

$result1 = fread($fp, 5);
var_dump(rewinddir($fp));
$result2 = fread($fp, 5);

echo "\n-- Check if rewinddir() has repositioned the file pointer --\n";
if ($result1 === $result2) {
	echo "rewinddir() works on file pointers\n";
} else {
	echo "rewinddir() does not work on file pointers\n";
}
?>
===DONE===
--EXPECTF--
*** Testing rewinddir() : usage variations ***

-- Open a file using fopen --
resource(%d) of type (stream)

Warning: rewinddir(): %d is not a valid Directory resource in %s on line %d
bool(false)

-- Check if rewinddir() has repositioned the file pointer --
rewinddir() does not work on file pointers
===DONE===
