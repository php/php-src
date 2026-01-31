--TEST--
posix_access() flag (mode) validation
--FILE--
<?php

$dir = __DIR__;
$testfile = "$dir/testfile.txt";

// Create a temporary file for valid access tests
file_put_contents($testfile, "hello");

// Invalid: negative mode
try {
    posix_access($testfile, -1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

// Invalid: mode with garbage bits
try {
    posix_access($testfile, 01000); // S_ISVTX bit (sticky)
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

// Invalid: mode with unrelated high bits
try {
    posix_access($testfile, 02000); // S_ISGID bit
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

// Valid: check read and write access
if (posix_access($testfile, POSIX_R_OK | POSIX_W_OK)) {
    echo "Read/write access OK\n";
}

// Valid: check file existence
if (posix_access($testfile, POSIX_F_OK)) {
    echo "File exists OK\n";
}

unlink($testfile);
?>
--EXPECTF--
posix_access(): Argument #2 ($flags) must be a combination of POSIX_F_OK, POSIX_R_OK, POSIX_W_OK, and POSIX_X_OK
posix_access(): Argument #2 ($flags) must be a combination of POSIX_F_OK, POSIX_R_OK, POSIX_W_OK, and POSIX_X_OK
posix_access(): Argument #2 ($flags) must be a combination of POSIX_F_OK, POSIX_R_OK, POSIX_W_OK, and POSIX_X_OK
Read/write access OK
File exists OK
