--TEST--
Test Uri\WhatWg\url_percent_encode() - username - special code points in the percent-encode set
--FILE--
<?php

var_dump(Uri\WhatWg\url_percent_encode("[#=? `]", Uri\WhatWg\UrlPercentEncodingMode::Username));

?>
--EXPECT--
string(21) "%5B%23%3D%3F%20%60%5D"
