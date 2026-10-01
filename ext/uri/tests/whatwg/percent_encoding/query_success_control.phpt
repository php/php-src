--TEST--
Test Uri\WhatWg\url_percent_encode() - query - control code points
--FILE--
<?php

var_dump(Uri\WhatWg\url_percent_encode("\x11", Uri\WhatWg\UrlPercentEncodingMode::Query));

?>
--EXPECT--
string(3) "%11"
