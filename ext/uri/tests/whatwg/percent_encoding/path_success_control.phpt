--TEST--
Test Uri\WhatWg\url_percent_encode() - path - control code points
--FILE--
<?php

var_dump(Uri\WhatWg\url_percent_encode("\x11", Uri\WhatWg\UrlPercentEncodingMode::Path));

?>
--EXPECT--
string(3) "%11"
