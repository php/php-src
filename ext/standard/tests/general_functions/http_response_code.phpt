--TEST--
Test http_response_code basic functionality
--CREDITS--
Gabriel Caruso (carusogabriel@php.net)
--SKIPIF--
<?php
if (getenv('SKIP_REPEAT')) {
    /* The http_response_code leaks across repeats. Technically that's a bug, but it's something
     * that only affects the CLI repeat option, where the response code is not meaningful in the
     * first place. Other SAPIs will properly reset the response code for each request. */
    die('skip Not repeatable');
}
?>
--FILE--
<?php
var_dump(
    // Get the current default response code
    http_response_code(),
    // Set a response code
    http_response_code(201),
    // Get the new response code
    http_response_code()
);
echo "Now we've sent the headers\n";
var_dump(
    // This should fail
    http_response_code(500)
);
?>
--EXPECTF--
bool(false)
bool(true)
int(201)
Now we've sent the headers

Warning: http_response_code(): Cannot set response code - headers already sent (output started at %s:%d) in %s on line %d
bool(false)
