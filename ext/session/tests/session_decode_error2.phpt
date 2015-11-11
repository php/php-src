--TEST--
Test session_decode() function : error functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/* 
 * Prototype : string session_decode(void)
 * Description : Decodes session data from a string
 * Source code : ext/session/session.c 
 */

echo "*** Testing session_decode() : error functionality ***\n";
$data = "foo|a:3:{i:0;i:1;i:1;i:2;i:2;i:3;}guff|R:1;blah|R:1;";

var_dump(session_start());
for($index = 0; $index < strlen($data); $index++) {
    echo "\n-- Iteration $index --\n";
    $encoded = substr($data, 0, $index);
    var_dump(session_decode($encoded));
    var_dump($_SESSION);
};

var_dump(session_destroy());
echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_decode() : error functionality ***
bool(true)

-- Iteration 0 --
bool(true)
array(0) {
}

-- Iteration 1 --
bool(true)
array(0) {
}

-- Iteration 2 --
bool(true)
array(0) {
}

-- Iteration 3 --
bool(true)
array(0) {
}

-- Iteration 4 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s%esession_decode_error2.php on line %d
bool(false)
array(0) {
}

-- Iteration 5 --
bool(false)
array(0) {
}

-- Iteration 6 --
bool(false)
array(0) {
}

-- Iteration 7 --
bool(false)
array(0) {
}

-- Iteration 8 --
bool(false)
array(0) {
}

-- Iteration 9 --
bool(false)
array(0) {
}

-- Iteration 10 --
bool(false)
array(0) {
}

-- Iteration 11 --
bool(false)
array(0) {
}

-- Iteration 12 --
bool(false)
array(0) {
}

-- Iteration 13 --
bool(false)
array(0) {
}

-- Iteration 14 --
bool(false)
array(0) {
}

-- Iteration 15 --
bool(false)
array(0) {
}

-- Iteration 16 --
bool(false)
array(0) {
}

-- Iteration 17 --
bool(false)
array(0) {
}

-- Iteration 18 --
bool(false)
array(0) {
}

-- Iteration 19 --
bool(false)
array(0) {
}

-- Iteration 20 --
bool(false)
array(0) {
}

-- Iteration 21 --
bool(false)
array(0) {
}

-- Iteration 22 --
bool(false)
array(0) {
}

-- Iteration 23 --
bool(false)
array(0) {
}

-- Iteration 24 --
bool(false)
array(0) {
}

-- Iteration 25 --
bool(false)
array(0) {
}

-- Iteration 26 --
bool(false)
array(0) {
}

-- Iteration 27 --
bool(false)
array(0) {
}

-- Iteration 28 --
bool(false)
array(0) {
}

-- Iteration 29 --
bool(false)
array(0) {
}

-- Iteration 30 --
bool(false)
array(0) {
}

-- Iteration 31 --
bool(false)
array(0) {
}

-- Iteration 32 --
bool(false)
array(0) {
}

-- Iteration 33 --
bool(false)
array(0) {
}

-- Iteration 34 --
bool(false)
array(0) {
}

-- Iteration 35 --
bool(false)
array(0) {
}

-- Iteration 36 --
bool(false)
array(0) {
}

-- Iteration 37 --
bool(false)
array(0) {
}

-- Iteration 38 --
bool(false)
array(0) {
}

-- Iteration 39 --
bool(false)
array(0) {
}

-- Iteration 40 --
bool(false)
array(0) {
}

-- Iteration 41 --
bool(false)
array(0) {
}

-- Iteration 42 --
bool(false)
array(0) {
}

-- Iteration 43 --
bool(false)
array(0) {
}

-- Iteration 44 --
bool(false)
array(0) {
}

-- Iteration 45 --
bool(false)
array(0) {
}

-- Iteration 46 --
bool(false)
array(0) {
}

-- Iteration 47 --
bool(false)
array(0) {
}

-- Iteration 48 --
bool(false)
array(0) {
}

-- Iteration 49 --
bool(false)
array(0) {
}

-- Iteration 50 --
bool(false)
array(0) {
}

-- Iteration 51 --
bool(false)
array(0) {
}

Warning: session_destroy(): Trying to destroy uninitialized session in %s%esession_decode_error2.php on line %d
bool(false)
Done
