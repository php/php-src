--TEST--
Test curl_copy_handle() function with basic functionality
--CREDITS--
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--SKIPIF--
<?php
    if (!extension_loaded("curl")) exit("skip curl extension not loaded");
?>
--FILE--
<?php
echo "*** Testing curl_copy_handle(): basic ***\n";

// create a new cURL resource
$ch = curl_init();

// set URL and other appropriate options
curl_setopt($ch, CURLOPT_URL, 'http://www.example.com/');
curl_setopt($ch, CURLOPT_HEADER, 0);

// copy the handle
$ch2 = curl_copy_handle($ch);

var_dump(curl_getinfo($ch) === curl_getinfo($ch2));
?>
--EXPECT--
*** Testing curl_copy_handle(): basic ***
bool(true)
