--TEST--
Test Uri\WhatWg\url_percent_encode() - form query - special code points in the percent-encode set
--FILE--
<?php

var_dump(Uri\WhatWg\url_percent_encode('"#<>&+,', Uri\WhatWg\UrlPercentEncodingMode::FormQuery));

?>
--EXPECT--
string(21) "%22%23%3C%3E%26%2B%2C"
