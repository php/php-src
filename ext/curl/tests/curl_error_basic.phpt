--TEST--
curl_error() function - basic test for curl_error using a fake url
--CREDITS--
Mattijs Hoitink mattijshoitink@gmail.com
#Testfest Utrecht 2009
--SKIPIF--
<?php if (!extension_loaded("curl")) print "skip"; ?>
--FILE--
<?php
/*
 * Prototype:     string curl_error(resource $ch)
 * Description:   Returns a clear text error message for the last cURL operation.
 * Source:        ext/curl/interface.c
 * Documentation: http://wiki.php.net/qa/temp/ext/curl
 */
 
// Fake URL to trigger an error
$url = "fakeURL";

echo "== Testing curl_error with a fake URL ==\n";

// cURL handler
$ch = curl_init($url);

ob_start(); // start output buffering
curl_exec($ch);
echo "Error: " . curl_error($ch);
curl_close($ch);

?>
--EXPECT--
== Testing curl_error with a fake URL ==
Error: Couldn't resolve host 'fakeURL'
