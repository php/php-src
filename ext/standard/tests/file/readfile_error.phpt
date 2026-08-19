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
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump( readfile(false) );  // boolean false as $filename
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "\n-- Testing readfile() with non-existent file --\n";
$non_existent_file = __DIR__."/non_existent_file.tmp";
var_dump( readfile($non_existent_file) );

echo "Done\n";
?>
--EXPECTF--
*** Test readfile(): error conditions ***

-- Testing readfile() with invalid arguments --
ValueError: Path must not be empty
ValueError: Path must not be empty

-- Testing readfile() with non-existent file --

Warning: readfile(): Failed to open stream: %s in %s on line %d
bool(false)
Done
