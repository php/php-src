--TEST--
GHSA-mg3p-mxj9-g2mm (Uri\Rfc3986\Uri recomposed to INT_MAX - 1 characters)
--SKIPIF--
<?php
if (!getenv('RUN_RESOURCE_HEAVY_TESTS')) die('skip resource-heavy test');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
if (PHP_INT_SIZE !== 8) die("skip this test is for 64bit platforms only");
$memInfo = @file_get_contents('/proc/meminfo');
if ($memInfo && preg_match('/MemAvailable:\s+(\d+) kB/', $memInfo, $m) && $m[1] < 5 * 1024 * 1024) {
    die('skip Insufficient available memory (less than 5 GiB)');
}
?>
--CONFLICTS--
all
--INI--
memory_limit=-1
--FILE--
<?php
$length = 2147483646; // 32-bit INT_MAX - 1
// Parsing a single relative path segment is cheap under sanitizers.
$string = (new Uri\Rfc3986\Uri(str_repeat("x", $length)))->toRawString();
var_dump(strlen($string));
var_dump(strcmp($string, str_repeat("x", $length)) === 0);
?>
--EXPECT--
int(2147483646)
bool(true)
