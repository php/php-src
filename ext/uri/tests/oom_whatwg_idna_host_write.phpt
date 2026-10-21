--TEST--
Uri\WhatWg\Url::withHost() with a long non-ASCII host reports the memory limit
--EXTENSIONS--
uri
--INI--
memory_limit=8M
--FILE--
<?php
$host = str_repeat("\u{e4}", (int) (1.5 * 1024 * 1024));

$url = Uri\WhatWg\Url::parse("http://example.com/");
$url->withHost($host);

echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
