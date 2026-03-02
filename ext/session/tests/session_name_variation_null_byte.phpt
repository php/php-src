--TEST--
Test session_name() function : null byte in session name
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

try {
    var_dump(session_name("AB\0CD"));
    var_dump(session_start());
} catch (ValueError $e) {
    echo $e->getMessage(). "\n";
}

echo "Done";
?>
--EXPECT--
session_name(): Argument #1 ($name) must not contain any null bytes
Done
