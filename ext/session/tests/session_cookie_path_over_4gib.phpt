--TEST--
session_start() with a Set-Cookie header longer than 4 GiB
--EXTENSIONS--
session
--SKIPIF--
<?php
include('skipif.inc');
if (!getenv('RUN_RESOURCE_HEAVY_TESTS')) die('skip resource-heavy test');
if (PHP_INT_SIZE != 8) die('skip this test is for 64bit platforms only');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow tests excluded by request');
?>
--INI--
memory_limit=-1
session.use_cookies=1
session.use_only_cookies=1
session.cache_limiter=
session.save_handler=files
--FILE--
<?php
// The new line/NUL check in sapi_header_op() used a 32-bit index, so a header
// line of 4 GiB or more made it loop forever.
ini_set('session.cookie_path', '/' . str_repeat('p', 4294967296));
var_dump(session_start());
session_destroy();
?>
--EXPECT--
bool(true)
