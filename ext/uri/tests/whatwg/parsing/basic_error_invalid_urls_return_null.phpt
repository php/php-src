--TEST--
Test Uri\WhatWg\Url parsing - basic - parse() returns null for invalid URLs
--FILE--
<?php

var_dump(Uri\WhatWg\Url::parse(""));
var_dump(Uri\WhatWg\Url::parse("192.168/contact.html", null));
var_dump(Uri\WhatWg\Url::parse(":", null));
var_dump(Uri\WhatWg\Url::parse("/page:1"));

?>
--EXPECT--
NULL
NULL
NULL
NULL
