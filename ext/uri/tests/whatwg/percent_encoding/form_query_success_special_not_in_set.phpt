--TEST--
Test Uri\WhatWg\url_percent_encode() - form query - special code points not in the percent-encode set
--FILE--
<?php

var_dump(Uri\WhatWg\url_percent_encode("*-._", Uri\WhatWg\UrlPercentEncodingMode::FormQuery));

?>
--EXPECT--
string(4) "*-._"
