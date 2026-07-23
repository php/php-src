--TEST--
Test Uri\WhatWg\url_percent_encode() - opaque path - percent sign code point
--FILE--
<?php

var_dump(Uri\WhatWg\url_percent_encode("WHATWG%20url", Uri\WhatWg\UrlPercentEncodingMode::OpaquePath));

?>
--EXPECT--
string(14) "WHATWG%2520url"
