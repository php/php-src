--TEST--
CurlUrl::set() with relative URLs
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073e00) die('skip requires curl >= 7.62.0');
?>
--FILE--
<?php

$url = new CurlUrl('https://www.php.net/original/path?with=query');
var_dump((string) $url);

$url->set('/another/path?with?another=query');
var_dump((string) $url);

?>
--EXPECT--
string(44) "https://www.php.net/original/path?with=query"
string(51) "https://www.php.net/another/path?with?another=query"
