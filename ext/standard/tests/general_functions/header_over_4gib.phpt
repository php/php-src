--TEST--
header() with a header line longer than 4 GiB
--SKIPIF--
<?php
if (!getenv('RUN_RESOURCE_HEAVY_TESTS')) die('skip resource-heavy test');
if (PHP_INT_SIZE != 8) die('skip this test is for 64bit platforms only');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow tests excluded by request');
if (PHP_OS === 'FreeBSD') die('skip Worker does not handle OOM gracefully');
?>
--CONFLICTS--
all
--INI--
memory_limit=-1
--FILE--
<?php
// The new line/NUL check in sapi_header_op() used a 32-bit index, so a header
// line of 4 GiB or more made it loop forever.
header('X-Foo: ' . str_repeat('a', 2**32) . "\nX-Bar: b");
echo "Done\n";
?>
--EXPECTF--
Warning: Header may not contain more than a single header, new line detected in %s on line %d
Done
