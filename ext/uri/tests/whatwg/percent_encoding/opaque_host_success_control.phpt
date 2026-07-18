--TEST--
Test Uri\WhatWg\url_percent_encode() - opaque host - control code points
--FILE--
<?php

var_dump(Uri\WhatWg\url_percent_encode("\x11", Uri\WhatWg\UrlPercentEncodingMode::OpaqueHost));

?>
--EXPECT--
string(3) "%11"
