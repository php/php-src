--TEST--
Test session_decode() function : error functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_decode() : error functionality ***\n";
$data = "foo|a:3:{i:0;i:1;i:1;i:2;i:2;i:3;}guff|R:1;blah|R:1;";

var_dump(session_start());
for($index = 0; $index < strlen($data); $index++) {
    if(session_status() != PHP_SESSION_ACTIVE) { session_start(); }
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

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 2 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 3 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 4 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 5 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 6 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 7 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 8 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 9 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 10 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 11 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 12 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 13 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 14 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 15 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 16 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 17 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 18 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 19 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 20 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 21 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 22 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 23 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 24 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 25 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 26 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 27 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 28 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 29 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 30 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 31 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 32 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 33 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 34 --
bool(true)
array(1) {
  ["foo"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}

-- Iteration 35 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 36 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 37 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 38 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 39 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 40 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 41 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 42 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 43 --
bool(true)
array(2) {
  ["foo"]=>
  &array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  ["guff"]=>
  &array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}

-- Iteration 44 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 45 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 46 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 47 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 48 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 49 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 50 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

-- Iteration 51 --

Warning: session_decode(): Failed to decode session object. Session has been destroyed in %s on line %d
bool(false)
array(0) {
}

Warning: session_destroy(): Trying to destroy uninitialized session in %s on line %d
bool(false)
Done
