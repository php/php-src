--TEST--
Test readfile() function: error conditions
--FILE--
<?php
$context = stream_context_create();

echo "*** Test readfile(): error conditions ***\n";

echo "\n-- Testing readfile() with invalid arguments --\n";
// invalid arguments
try {
    var_dump( readfile('') );  // empty string as $filename
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump( readfile(false) );  // boolean false as $filename
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "\n-- Testing readfile() with non-existent file --\n";
$non_existent_file = __DIR__."/non_existent_file.tmp";
var_dump( readfile($non_existent_file) );

echo "Done\n";
?>
--EXPECTF--
*** Test readfile(): error conditions ***

-- Testing readfile() with invalid arguments --
Path cannot be empty
Path cannot be empty

-- Testing readfile() with non-existent file --

Warning: readfile(%s/non_existent_file.tmp): Failed to open stream: %s in %s on line %d
bool(false)
Done
