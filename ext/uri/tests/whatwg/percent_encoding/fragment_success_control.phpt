--TEST--
Test Uri\WhatWg\url_percent_encode() - fragment - control code points
--FILE--
<?php

var_dump(Uri\WhatWg\url_percent_encode("\x11", Uri\WhatWg\UrlPercentEncodingMode::Fragment));

?>
--EXPECT--
string(3) "%11"
