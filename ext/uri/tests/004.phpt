--TEST--
Parse invalid URLs
--EXTENSIONS--
uri
--FILE--
<?php

try {
    new Uri\WhatWg\Url("");
} catch (Uri\WhatWg\InvalidUrlException $e) {
    echo $e->getMessage() . "\n";
}

var_dump(Uri\WhatWg\Url::parse(""));

var_dump(Uri\WhatWg\Url::parse("192.168/contact.html", null));

var_dump(Uri\WhatWg\Url::parse("http://RuPaul's Drag Race All Stars 7 Winners Cast on This Season's", null));

?>
--EXPECTF--
URL parsing failed
NULL
NULL
NULL
