--TEST--
Test Uri\WhatWg\Url parsing - path - space before the query or fragment of an opaque path
--FILE--
<?php

foreach (["data:x ?q", "data:x #f", "foo:a b ?c", "data:x  ?q"] as $input) {
    $url = new Uri\WhatWg\Url($input);
    var_dump($url->getPath());
}

?>
--EXPECT--
string(4) "x%20"
string(4) "x%20"
string(6) "a b%20"
string(5) "x %20"
