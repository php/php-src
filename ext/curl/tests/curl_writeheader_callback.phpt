--TEST--
Test curl option CURLOPT_HEADERFUNCTION 
--CREDITS--
Mathieu Kooiman <mathieuk@gmail.com>
Dutch UG, TestFest 2009, Utrecht
--DESCRIPTION--
Hit the host identified by PHP_CURL_HTTP_REMOTE_SERVER and determine that the headers are sent to the callback specified for CURLOPT_HEADERFUNCTION. Different test servers specified for PHP_CURL_HTTP_REMOTE_SERVER might return different sets of headers. Just test for HTTP/1.1 200 OK.
--SKIPIF--
<?php if (!extension_loaded("curl") || false === getenv('PHP_CURL_HTTP_REMOTE_SERVER')) print "skip"; ?>
--FILE--
<?php

function curl_header_callback($curl_handle, $data)
{
	if (strtolower(substr($data,0, 4)) == 'http')
		echo $data;
}

$host = getenv('PHP_CURL_HTTP_REMOTE_SERVER');

$ch = curl_init();
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch, CURLOPT_HEADERFUNCTION, 'curl_header_callback');
curl_setopt($ch, CURLOPT_URL, $host);
curl_exec($ch);
curl_close($ch);

?>
--EXPECTF--
HTTP/1.1 %d %s
