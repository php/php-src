--TEST--
Test Uri\WhatWg\url_percent_encode() - query - space code point
--FILE--
<?php

var_dump(Uri\WhatWg\url_percent_encode("WHATWG url", Uri\WhatWg\UrlPercentEncodingMode::Query));

?>
--EXPECT--
string(12) "WHATWG%20url"
