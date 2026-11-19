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
$uri = new Uri\Rfc3986\Uri("a://b/" . str_repeat("x", 2147483646 - 6));

$string = $uri->toRawString();
var_dump(strlen($string));
var_dump(substr($string, 0, 8));
var_dump(strspn($string, "x", 6) === 2147483646 - 6);
?>
--EXPECT--
int(2147483646)
string(8) "a://b/xx"
bool(true)
