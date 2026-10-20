--TEST--
Test Uri\WhatWg\url_percent_encode() - special query - space code point
--FILE--
<?php

var_dump(Uri\WhatWg\url_percent_encode("WHATWG url", Uri\WhatWg\UrlPercentEncodingMode::SpecialQuery));

?>
--EXPECT--
string(12) "WHATWG%20url"
