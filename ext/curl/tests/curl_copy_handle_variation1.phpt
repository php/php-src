--TEST--
Test curl_copy_handle() change options in one handle
--CREDITS--
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--EXTENSIONS--
curl
--FILE--
<?php
echo "*** Testing curl_copy_handle(): basic ***\n";

// create a new cURL resource
$ch = curl_init();

// set URL and other appropriate options
curl_setopt($ch, CURLOPT_URL, 'http://www.example.com/');

// copy the handle
$ch2 = curl_copy_handle($ch);

// change the CURLOPT_URL for the second handle
curl_setopt($ch2, CURLOPT_URL, 'http://www.bar.com/');

var_dump(curl_getinfo($ch) === curl_getinfo($ch2));
?>
--EXPECT--
*** Testing curl_copy_handle(): basic ***
bool(false)
