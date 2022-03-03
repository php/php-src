--TEST--
Test curl option CURLOPT_HEADERFUNCTION
--CREDITS--
Mathieu Kooiman <mathieuk@gmail.com>
Dutch UG, TestFest 2009, Utrecht
--DESCRIPTION--
Hit the host and determine that the headers are sent to the callback specified for CURLOPT_HEADERFUNCTION. Different test servers might return different sets of headers. Just test for HTTP/1.1 200 OK.
--EXTENSIONS--
curl
--FILE--
<?php

function curl_header_callback($curl_handle, $data)
{
    if (strtolower(substr($data,0, 4)) == 'http')
        echo $data;
}

include 'server.inc';
$host = curl_cli_server_start();

$ch = curl_init();
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch, CURLOPT_HEADERFUNCTION, 'curl_header_callback');
curl_setopt($ch, CURLOPT_URL, $host);
curl_exec($ch);
curl_close($ch);

?>
--EXPECTF--
HTTP/1.1 %d %s
