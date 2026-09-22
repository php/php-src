--TEST--
Test Uri\WhatWg\url_percent_encode() - special query - control code points
--FILE--
<?php

var_dump(Uri\WhatWg\url_percent_encode("\x11", Uri\WhatWg\UrlPercentEncodingMode::SpecialQuery));

?>
--EXPECT--
string(3) "%11"
