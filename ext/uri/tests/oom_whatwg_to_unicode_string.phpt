--TEST--
Uri\WhatWg\Url::toUnicodeString() doesn't reach hard memory limit
--EXTENSIONS--
uri
--INI--
memory_limit=8M
--FILE--
<?php
$url = Uri\WhatWg\Url::parse("http://" . str_repeat("\u{e4}", 1200 * 1024) . "/");

$url->toUnicodeString();

echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
