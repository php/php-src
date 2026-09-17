--TEST--
Test Uri\WhatWg\url_percent_encode() - fragment - percent sign code point
--FILE--
<?php

var_dump(Uri\WhatWg\url_percent_encode("WHATWG%20url", Uri\WhatWg\UrlPercentEncodingMode::Username));

?>
--EXPECT--
string(14) "WHATWG%2520url"
