--TEST--
Test Uri\WhatWg\url_percent_encode() - special query - Unicode code points
--FILE--
<?php

var_dump(Uri\WhatWg\url_percent_encode("föő", Uri\WhatWg\UrlPercentEncodingMode::SpecialQuery));

?>
--EXPECT--
string(13) "f%C3%B6%C5%91"
