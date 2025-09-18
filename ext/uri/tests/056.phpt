--TEST--
Test Lexbor memory management
--EXTENSIONS--
uri
--FILE--
<?php

$urls = [];

for ($i = 0; $i < 1000; $i++) {
    $urls[] = Uri\WhatWg\Url::parse("https://example.com/{$i}/");
}

for ($i = 0; $i < 1000; $i++) {
    if ($urls[$i]->toAsciiString() !== "https://example.com/{$i}/") {
        die("FAIL");
    }
}

?>
==DONE==
--EXPECT--
==DONE==
