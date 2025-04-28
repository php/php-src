--TEST--
Test URI equality checks
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(new Uri\Rfc3986\Uri("https://example.com")->equals(new Uri\Rfc3986\Uri("https://example.com"))); // true: identical URIs
var_dump(new Uri\Rfc3986\Uri("https://example.com#foo")->equals(new Uri\Rfc3986\Uri("https://example.com#bar"), Uri\UriComparisonMode::ExcludeFragment)); // true: fragment differs, but fragment is excluded
var_dump(new Uri\Rfc3986\Uri("https://example.com#foo")->equals(new Uri\Rfc3986\Uri("https://example.com#bar"), Uri\UriComparisonMode::IncludeFragment)); // false: fragment differs and fragment is included
var_dump(new Uri\Rfc3986\Uri("https://example.com/foo/..")->equals(new Uri\Rfc3986\Uri("https://example.com"))); // false: first URI becomes https://example.com/ after normalization
var_dump(new Uri\Rfc3986\Uri("https://example.com/foo/..")->equals(new Uri\Rfc3986\Uri("https://example.com/"))); // true: both URIs are https://example.com/ after normalization
var_dump(new Uri\Rfc3986\Uri("http://example%2ecom/foo%2fb%61r")->equals(new Uri\Rfc3986\Uri("http://example%2ecom/foo/bar"))); // false: "/" in the path should not be decoded
var_dump(new Uri\Rfc3986\Uri("http://example%2ecom/foo/b%61r")->equals(new Uri\Rfc3986\Uri("http://example%2ecom/foo/bar"))); // true: percent-decoding during normalization gives same URIs

var_dump(new Uri\WhatWg\Url("https://example.com")->equals(new Uri\WhatWg\Url("https://example.com"))); // true: identical URIs
var_dump(new Uri\WhatWg\Url("https://example.com#foo")->equals(new Uri\WhatWg\Url("https://example.com#bar"), Uri\UriComparisonMode::ExcludeFragment)); // true: fragment differs, but fragment is excluded
var_dump(new Uri\WhatWg\Url("https://example.com#foo")->equals(new Uri\WhatWg\Url("https://example.com#bar"), Uri\UriComparisonMode::IncludeFragment)); // false: fragment differs and fragment is included
var_dump(new Uri\WhatWg\Url("https://example.com/foo/..")->equals(new Uri\WhatWg\Url("https://example.com"))); // true: both URIs are https://example.com/ after normalization
var_dump(new Uri\WhatWg\Url("https://example.com/foo/..")->equals(new Uri\WhatWg\Url("https://example.com/"))); // true: both URIs are https://example.com/ after normalization
var_dump(new Uri\WhatWg\Url("http://example%2ecom/foo%2fb%61r")->equals(new Uri\WhatWg\Url("http://example%2ecom/foo/bar"))); // false: WHATWG doesn't percent-decode the path during normalization
var_dump(new Uri\WhatWg\Url("http://example%2ecom/foo/b%61r")->equals(new Uri\WhatWg\Url("http://example.com/foo/b%61r"))); // true: WHATWG percent-decodes the host during normalization

?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
