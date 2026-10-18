--TEST--
Uri\WhatWg\Url validation errors don't reach hard memory limit
--EXTENSIONS--
uri
--INI--
memory_limit=16M
--FILE--
<?php
$errors = null;

new Uri\WhatWg\Url("https://example.com/" . str_repeat("%", 100 * 1024), null, $errors);

echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 16777216 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
