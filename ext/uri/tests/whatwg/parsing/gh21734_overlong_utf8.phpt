--TEST--
GH-21734: WHATWG URL parser rejects overlong UTF-8 and invalid continuation bytes
--FILE--
<?php

// Overlong hostname: %C1%A5 = overlong 'e', %C1%B6 = overlong 'v', etc.
// Must fail to parse (browsers reject at the UTF-8 decode step)
$url = Uri\WhatWg\Url::parse("http://%C1%A5%C1%B6%C1%A9%C1%AC.com/");
var_dump($url);

// Raw overlong 'a' in hostname
$url = Uri\WhatWg\Url::parse("http://\xC1\xA1.com/");
var_dump($url);

// Overlong '/' in path: raw bytes \xC0\xAF should not produce a path separator
$url = Uri\WhatWg\Url::parse("http://example.com/a\xC0\xAFb");
var_dump($url->getPath());

// Invalid continuation byte: \xC0 followed by ASCII 'A' (not 0x80-0xBF)
$url = Uri\WhatWg\Url::parse("http://example.com/\xC0\x41");
var_dump($url->getPath());

// Surrogate \xED\xA0\x80 = U+D800 (must be rejected)
$url = Uri\WhatWg\Url::parse("http://example.com/\xED\xA0\x80");
var_dump($url->getPath());

// Valid UTF-8 still works
$url = Uri\WhatWg\Url::parse("http://example.com/café");
var_dump($url->getPath());

// Valid IDN hostname still works
$url = Uri\WhatWg\Url::parse("http://münchen.de/");
var_dump($url->getAsciiHost());

?>
--EXPECT--
NULL
NULL
string(9) "/a%C0%AFb"
string(5) "/%C0A"
string(10) "/%ED%A0%80"
string(10) "/caf%C3%A9"
string(17) "xn--mnchen-3ya.de"
