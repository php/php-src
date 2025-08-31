--TEST--
Test rewinddir() function : usage variations - file pointers
--FILE--
<?php
/*
 * Pass a file pointer to rewinddir() to test behaviour
 */

echo "*** Testing rewinddir() : usage variations ***\n";

echo "\n-- Open a file using fopen --\n";
var_dump($fp = fopen(__FILE__, 'r'));

$result1 = fread($fp, 5);

try {
    var_dump(rewinddir($fp));
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}
$result2 = fread($fp, 5);

echo "\n-- Check if rewinddir() has repositioned the file pointer --\n";
if ($result1 === $result2) {
    echo "rewinddir() works on file pointers\n";
} else {
    echo "rewinddir() does not work on file pointers\n";
}
?>
--EXPECTF--
*** Testing rewinddir() : usage variations ***

-- Open a file using fopen --
resource(%d) of type (stream)
rewinddir(): Argument #1 ($dir_handle) must be a valid Directory resource

-- Check if rewinddir() has repositioned the file pointer --
rewinddir() does not work on file pointers
