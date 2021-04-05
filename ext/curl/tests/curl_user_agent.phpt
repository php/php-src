--TEST--
Test curl.user_agent ini entry
--INI--
curl.user_agent=test/1.0
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'server.inc';
$host = curl_cli_server_start();

$ch = curl_init($host);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLINFO_HEADER_OUT, true);
curl_exec($ch);
$info = curl_getinfo($ch);
echo $info['request_header'];
curl_close($ch);

ini_set('curl.user_agent', 'ini/1.0');

//Override with ini_set
$ch = curl_init($host);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLINFO_HEADER_OUT, true);
curl_exec($ch);
$info = curl_getinfo($ch);
echo $info['request_header'];
curl_close($ch);

//Override with CURLOPT
$ch = curl_init($host);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_USERAGENT, 'override/1.0');
curl_setopt($ch, CURLINFO_HEADER_OUT, true);
curl_exec($ch);
$info = curl_getinfo($ch);
echo $info['request_header'];
curl_close($ch);
?>
--EXPECTREGEX--
GET \/ HTTP\/1\.1
Host: localhost:[0-9]*
User-Agent: test\/1.0
Accept: \*\/\*

GET \/ HTTP\/1\.1
Host: localhost:[0-9]*
User-Agent: ini\/1.0
Accept: \*\/\*

GET \/ HTTP\/1\.1
Host: localhost:[0-9]*
User-Agent: override\/1.0
Accept: \*\/\*