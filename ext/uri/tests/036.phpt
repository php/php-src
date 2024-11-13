--TEST--
Test URI equality checks
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(Uri\Rfc3986Uri::create("https://example.com")->equalsTo(Uri\Rfc3986Uri::create("https://example.com")));
var_dump(Uri\Rfc3986Uri::create("https://example.com#foo")->equalsTo(Uri\Rfc3986Uri::create("https://example.com#bar"), true));
var_dump(Uri\Rfc3986Uri::create("https://example.com#foo")->equalsTo(Uri\Rfc3986Uri::create("https://example.com#bar"), false));
var_dump(Uri\Rfc3986Uri::create("https://example.com/")->equalsTo(Uri\WhatWgUri::create("https://example.com/")));
var_dump(Uri\Rfc3986Uri::create("https://example.com/foo/..")->equalsTo(Uri\Rfc3986Uri::create("https://example.com")));

var_dump(Uri\WhatWgUri::create("https://example.com")->equalsTo(Uri\WhatWgUri::create("https://example.com")));
var_dump(Uri\WhatWgUri::create("https://example.com#foo")->equalsTo(Uri\WhatWgUri::create("https://example.com#bar"), true));
var_dump(Uri\WhatWgUri::create("https://example.com#foo")->equalsTo(Uri\WhatWgUri::create("https://example.com#bar"), false));
var_dump(Uri\WhatWgUri::create("https://example.com/")->equalsTo(Uri\Rfc3986Uri::create("https://example.com/")));
var_dump(Uri\WhatWgUri::create("https://example.com/foo/..")->equalsTo(Uri\WhatWgUri::create("https://example.com")));

?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
