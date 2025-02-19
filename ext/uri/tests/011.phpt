--TEST--
Test encoding and normalization
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(Uri\Rfc3986\Uri::parse("http://////www.EXAMPLE.com:80")->toRawString());
var_dump(Uri\Rfc3986\Uri::parse("https://www.example.com/dir1/../dir2")->toRawString());
var_dump(Uri\Rfc3986\Uri::parse("https://你好你好"));
var_dump(Uri\Rfc3986\Uri::parse("https://０Ｘｃ０．０２５０．０１"));
var_dump(Uri\Rfc3986\Uri::parse("HttPs://0300.0250.0000.0001/path?query=foo%20bar")->toRawString());

var_dump(Uri\WhatWg\Url::parse("http://////www.EXAMPLE.com:80")->toAsciiString());
var_dump(Uri\WhatWg\Url::parse("https://www.example.com:443/dir1/../dir2")->toAsciiString());
var_dump(Uri\WhatWg\Url::parse("https://你好你好")->toAsciiString());
var_dump(Uri\WhatWg\Url::parse("https://你好你好")->toUnicodeString());
var_dump(Uri\WhatWg\Url::parse("https://０Ｘｃ０．０２５０．０１")->toAsciiString());
var_dump(Uri\WhatWg\Url::parse("HttPs://0300.0250.0000.0001/path?query=foo%20bar")->toAsciiString());

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
string(21) "https://你好你好/"
string(20) "https://192.168.0.1/"
string(40) "https://192.168.0.1/path?query=foo%20bar"
