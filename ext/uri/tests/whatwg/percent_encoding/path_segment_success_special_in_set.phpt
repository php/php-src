--TEST--
Test Uri\WhatWg\url_percent_encode() - path segment - special code points in the percent-encode set
--FILE--
<?php

var_dump(Uri\WhatWg\url_percent_encode(' "#<>?^`{}/', Uri\WhatWg\UrlPercentEncodingMode::PathSegment));

?>
--EXPECT--
string(33) "%20%22%23%3C%3E%3F%5E%60%7B%7D%2F"
