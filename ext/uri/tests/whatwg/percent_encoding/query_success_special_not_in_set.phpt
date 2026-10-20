--TEST--
Test Uri\WhatWg\url_percent_encode() - query - special code points not in the percent-encode set
--FILE--
<?php

var_dump(Uri\WhatWg\url_percent_encode("[@?&]'", Uri\WhatWg\UrlPercentEncodingMode::Query));

?>
--EXPECT--
string(6) "[@?&]'"
