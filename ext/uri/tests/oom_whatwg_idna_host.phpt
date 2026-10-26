--TEST--
Uri\WhatWg\Url with a long non-ASCII host doesn't reach hard memory limit
--EXTENSIONS--
uri
--INI--
memory_limit=8M
--FILE--
<?php
/* A non-ASCII host goes through IDNA */
$host = str_repeat("\u{e4}", (int) (1.5 * 1024 * 1024));

Uri\WhatWg\Url::parse("http://" . $host . "/");

echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
