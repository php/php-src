--TEST--
deflate_add() output buffer pre-allocation doesn't reach hard limit
--EXTENSIONS--
zlib
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

// Allocate the deflate state first so that it counts towards the baseline.
$ctx = deflate_init(ZLIB_ENCODING_RAW);

// Fill the heap to just under the memory limit.
$subject = str_repeat("a", $limit - memory_get_usage(true) - 64 * 1024);

// deflate_add() pre-allocates PHP_ZLIB_BUFFER_SIZE_GUESS() bytes for its
// output, 1.5% more than the chunk, which leaves it too big for the gap that is
// left before the hard limit.
$out_size = (int) (strlen($subject) * 1.015) + 23;
var_dump($out_size > $hard - memory_get_usage(true));

deflate_add($ctx, $subject, ZLIB_FINISH);
echo "not reached\n";
?>
--EXPECTF--
bool(true)

Fatal error: Allowed memory size of 536870912 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
