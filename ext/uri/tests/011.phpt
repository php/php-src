--TEST--
Test encoding and normalization
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(Uri\Rfc3986Uri::create("http://////www.EXAMPLE.com:80")->__toString());
var_dump(Uri\Rfc3986Uri::create("https://www.example.com/dir1/../dir2")->__toString());
var_dump(Uri\Rfc3986Uri::create("https://你好你好"));
var_dump(Uri\Rfc3986Uri::create("https://０Ｘｃ０．０２５０．０１"));
var_dump(Uri\Rfc3986Uri::create("HttPs://0300.0250.0000.0001/path?query=foo%20bar")->__toString());

var_dump(Uri\WhatWgUri::create("http://////www.EXAMPLE.com:80")->__toString());
var_dump(Uri\WhatWgUri::create("https://www.example.com:443/dir1/../dir2")->__toString());
var_dump(Uri\WhatWgUri::create("https://你好你好")->__toString());
var_dump(Uri\WhatWgUri::create("https://０Ｘｃ０．０２５０．０１")->__toString());
var_dump(Uri\WhatWgUri::create("HttPs://0300.0250.0000.0001/path?query=foo%20bar")->__toString());

?>
--EXPECT--
string(29) "http://////www.EXAMPLE.com:80"
string(36) "https://www.example.com/dir1/../dir2"
NULL
NULL
string(48) "HttPs://0300.0250.0000.0001/path?query=foo%20bar"
string(23) "http://www.example.com/"
string(28) "https://www.example.com/dir2"
string(23) "https://xn--6qqa088eba/"
string(20) "https://192.168.0.1/"
string(40) "https://192.168.0.1/path?query=foo%20bar"
