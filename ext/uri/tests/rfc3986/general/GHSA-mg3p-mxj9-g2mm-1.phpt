--TEST--
GHSA-mg3p-mxj9-g2mm (Uri\Rfc3986\Uri recomposed to exactly INT_MAX characters)
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
$uri = new Uri\Rfc3986\Uri("a://b/" . str_repeat("x", 2147483647 - 6));

try {
    $uri->toRawString();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

try {
    $uri->toString();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
Uri\UriError: Cannot recompose Uri\Rfc3986\Uri to a string
Uri\UriError: Cannot recompose Uri\Rfc3986\Uri to a string
