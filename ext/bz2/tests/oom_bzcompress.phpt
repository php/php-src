--TEST--
bzcompress() output buffer pre-allocation doesn't reach hard limit
--EXTENSIONS--
bz2
--INI--
memory_limit=512M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--FILE--
<?php
$limit = 512 * 1024 * 1024;
$hard = 2 * $limit;

// Fill the heap to just under the memory limit.
$subject = str_repeat("a", $limit - memory_get_usage(true) - 64 * 1024);

// bzcompress() pre-allocates source + 1% + 600 bytes for its output, which
// leaves it about 1% too big for the gap that is left before the hard limit.
$dest_len = (int) (strlen($subject) + 0.01 * strlen($subject) + 600);
var_dump($dest_len > $hard - memory_get_usage(true));

bzcompress($subject);
echo "not reached\n";
?>
--EXPECTF--
bool(true)

Fatal error: Allowed memory size of 536870912 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
