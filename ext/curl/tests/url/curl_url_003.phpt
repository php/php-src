--TEST--
CurlUrl::get() with and without flags
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073e00) die('skip requires curl >= 7.62.0');
?>
--FILE--
<?php

var_dump(
        (new CurlUrl('https://www.example.com/'))->get(),
        (new CurlUrl('https://www.example.com/'))->get(CurlUrl::DEFAULT_PORT),
);  
 
?> 
--EXPECT--
string(24) "https://www.example.com/"
string(28) "https://www.example.com:443/"
