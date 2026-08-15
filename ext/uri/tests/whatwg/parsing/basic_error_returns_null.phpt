--TEST--
Test Uri\WhatWg\Url::parse() returns null for malformed URLs
--FILE--
<?php

var_dump(Uri\WhatWg\Url::parse(""));
var_dump(Uri\WhatWg\Url::parse("192.168/contact.html", null));
var_dump(Uri\WhatWg\Url::parse("http://RuPaul's Drag Race All Stars 7 Winners Cast on This Season's", null));
var_dump(Uri\WhatWg\Url::parse("/page:1"));

?>
--EXPECT--
NULL
NULL
NULL
NULL
