--TEST--
Test Uri\WhatWg\url_percent_encode() - special query - special code points in the percent-encode set
--FILE--
<?php

var_dump(Uri\WhatWg\url_percent_encode(" '\"#<>", Uri\WhatWg\UrlPercentEncodingMode::SpecialQuery));

?>
--EXPECT--
string(18) "%20%27%22%23%3C%3E"
