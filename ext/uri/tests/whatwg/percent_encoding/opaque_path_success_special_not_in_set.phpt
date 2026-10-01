--TEST--
Test Uri\WhatWg\url_percent_encode() - opaque path - special code points not in the percent-encode set
--FILE--
<?php

var_dump(Uri\WhatWg\url_percent_encode(" \"#<>?^`{}@'$+,", Uri\WhatWg\UrlPercentEncodingMode::OpaquePath));

?>
--EXPECT--
string(15) " "#<>?^`{}@'$+,"
