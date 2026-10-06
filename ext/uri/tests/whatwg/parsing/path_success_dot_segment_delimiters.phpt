--TEST--
Test Uri\WhatWg\Url parsing - path - query and fragment after a dot segment
--FILE--
<?php

foreach ([
    "https://example.com/..#frag",
    "https://example.com/caf\u{e9}/..#frag",
    "https://example.com/..?q=1",
    "https://example.com/%?q",
] as $input) {
    $url = new Uri\WhatWg\Url($input);
    var_dump($url->getPath(), $url->getQuery(), $url->getFragment());
}

?>
--EXPECT--
string(1) "/"
NULL
string(4) "frag"
string(1) "/"
NULL
string(4) "frag"
string(1) "/"
string(3) "q=1"
NULL
string(2) "/%"
string(1) "q"
NULL
