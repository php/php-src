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
?>
--EXPECT--
bool(false)
bool(true)
int(201)
