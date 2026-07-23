--TEST--
Test Uri\WhatWg\url_percent_encode() - password - control code points
--FILE--
<?php

var_dump(Uri\WhatWg\url_percent_encode("\x11", Uri\WhatWg\UrlPercentEncodingMode::PathSegment));

?>
--EXPECT--
string(3) "%11"
