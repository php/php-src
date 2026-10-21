--TEST--
Uri\WhatWg\Url::resolve() doesn't reach hard memory limit
--EXTENSIONS--
uri
--INI--
memory_limit=8M
--FILE--
<?php
/* The relative reference carries the host, so resolving it runs IDNA while
 * the base URL is still held. */
$base = Uri\WhatWg\Url::parse("http://example.com/");

$base->resolve("//" . str_repeat("\u{e4}", 1200 * 1024) . "/");

echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
