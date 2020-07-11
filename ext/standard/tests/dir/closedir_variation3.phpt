--TEST--
Test closedir() function : usage variations - close a file pointer
--FILE--
<?php
/*
 * Create a file pointer using fopen() then try to close it using closedir()
 */

echo "*** Testing closedir() : usage variations ***\n";

echo "\n-- Open a file using fopen() --\n";
var_dump($fp = fopen(__FILE__, 'r'));

echo "\n-- Try to close the file pointer using closedir() --\n";
try {
    var_dump(closedir($fp));
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}
echo "\n-- Check file pointer: --\n";
var_dump($fp);

if(is_resource($fp)) {
    fclose($fp);
}
?>
--EXPECTF--
*** Testing closedir() : usage variations ***

-- Open a file using fopen() --
resource(%d) of type (stream)

-- Try to close the file pointer using closedir() --
closedir(): Argument #1 ($dir_handle) must be a valid Directory resource

-- Check file pointer: --
resource(%d) of type (stream)
