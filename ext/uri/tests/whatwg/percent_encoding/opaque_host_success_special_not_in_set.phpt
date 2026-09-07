--TEST--
Test Uri\WhatWg\url_percent_encode() - opaque host - special code points not in the percent-encode set
--FILE--
<?php

var_dump(Uri\WhatWg\url_percent_encode(" \"#<>?^`{}@'$+,", Uri\WhatWg\UrlPercentEncodingMode::OpaqueHost));

?>
--EXPECT--
string(15) " "#<>?^`{}@'$+,"
