--TEST--
CurlUrl::__construt()
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073e00) die('skip requires curl >= 7.62.0');
?>
--FILE--
<?php

$url1 = new CurlUrl();
$url2 = new CurlUrl(null);
$url3 = new CurlUrl('https://www.google.com');
$url4 = new CurlUrl('foobar://www.google.com', CurlUrl::NON_SUPPORT_SCHEME);

?>
==DONE==
--EXPECT--
==DONE==
