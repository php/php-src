--TEST--
Test encoding and normalization
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(Uri\WhatWg\Url::parse("http://////www.EXAMPLE.com:80")->toAsciiString());
var_dump(Uri\WhatWg\Url::parse("https://www.example.com:443/dir1/../dir2")->toAsciiString());
var_dump(Uri\WhatWg\Url::parse("https://你好你好")->toAsciiString());
var_dump(Uri\WhatWg\Url::parse("https://你好你好")->toUnicodeString());
var_dump(Uri\WhatWg\Url::parse("https://０Ｘｃ０．０２５０．０１")->toAsciiString());
var_dump(Uri\WhatWg\Url::parse("HttPs://0300.0250.0000.0001/path?query=foo%20bar")->toAsciiString());

?>
--EXPECT--
string(23) "http://www.example.com/"
string(28) "https://www.example.com/dir2"
string(23) "https://xn--6qqa088eba/"
string(21) "https://你好你好/"
string(20) "https://192.168.0.1/"
string(40) "https://192.168.0.1/path?query=foo%20bar"
