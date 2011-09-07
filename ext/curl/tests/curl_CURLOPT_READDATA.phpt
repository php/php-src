--TEST--
Test CURLOPT_READDATA without a callback function
--CREDITS--
Mattijs Hoitink mattijshoitink@gmail.com
#Testfest Utrecht 2009
--SKIPIF--
<?php if (!extension_loaded("curl") || false === getenv('PHP_CURL_HTTP_REMOTE_SERVER')) print "skip need PHP_CURL_HTTP_REMOTE_SERVER environment variable"; ?>
--FILE--
<?php

// The URL to POST to
$url = getenv('PHP_CURL_HTTP_REMOTE_SERVER') . '/get.php?test=post';

// Create a temporary file to read the data from
$tempname = tempnam(sys_get_temp_dir(), 'CURL_DATA');
$datalen = file_put_contents($tempname, "hello=world&smurf=blue");

ob_start();

$ch = curl_init($url);
curl_setopt($ch, CURLOPT_URL, $url);
curl_setopt($ch, CURLOPT_POST, true);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_READDATA, fopen($tempname, 'rb'));
curl_setopt($ch, CURLOPT_HTTPHEADER, array('Expect:', "Content-Length: {$datalen}"));

if (false === $response = curl_exec($ch)) {
    echo 'Error #' . curl_errno($ch) . ': ' . curl_error($ch);
} else {
    echo $response;
}

curl_close($ch);

// Clean the temporary file
@unlink($tempname);

--EXPECT--
array(2) {
  ["hello"]=>
  string(5) "world"
  ["smurf"]=>
  string(4) "blue"
}
