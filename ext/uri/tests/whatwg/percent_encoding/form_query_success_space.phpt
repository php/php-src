--TEST--
Test Uri\WhatWg\url_percent_encode() - form query - space code point
--FILE--
<?php

var_dump(Uri\WhatWg\url_percent_encode("WHATWG url", Uri\WhatWg\UrlPercentEncodingMode::FormQuery));

?>
--EXPECT--
string(10) "WHATWG+url"
