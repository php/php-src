--TEST--
Test Uri\WhatWg\url_percent_encode() - fragment - special code points in the percent-encode set
--FILE--
<?php

var_dump(Uri\WhatWg\url_percent_encode(' "<>`', Uri\WhatWg\UrlPercentEncodingMode::Fragment));

?>
--EXPECT--
string(15) "%20%22%3C%3E%60"
