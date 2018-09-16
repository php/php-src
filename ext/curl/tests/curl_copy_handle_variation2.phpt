--TEST--
Test curl_copy_handle() add options to the handles
--CREDITS--
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--SKIPIF--
<?php
	if (!extension_loaded("curl")) exit("skip curl extension not loaded");
?>
--DESCRIPTION--
the only way to test if a option is setten on a curl handle is using the curl_getinfo() function.
but this can only check on a limited amount of options...
--FILE--
<?php
echo "*** Testing curl_copy_handle(): add options after copy ***\n";

// create a new cURL resource
$ch = curl_init();

// copy the handle
$ch2 = curl_copy_handle($ch);
var_dump(curl_getinfo($ch) === curl_getinfo($ch2));

// add some CURLOPT to the second handle
curl_setopt($ch2, CURLOPT_URL, 'http://www.example.com/');

var_dump(curl_getinfo($ch) === curl_getinfo($ch2));

// add same CURLOPT to the first handle
curl_setopt($ch, CURLOPT_URL, 'http://www.example.com/');
var_dump(curl_getinfo($ch) === curl_getinfo($ch2));

// change a CURLOPT in the second handle
curl_setopt($ch2, CURLOPT_URL, 'http://www.bar.com/');
var_dump(curl_getinfo($ch) === curl_getinfo($ch2));
?>
===DONE===
--EXPECT--
*** Testing curl_copy_handle(): add options after copy ***
bool(true)
bool(false)
bool(true)
bool(false)
===DONE===
