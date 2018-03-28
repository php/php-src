--TEST--
Test http_response_code basic functionality
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
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
