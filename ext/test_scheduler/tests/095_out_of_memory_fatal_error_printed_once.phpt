--TEST--
test_scheduler: an out-of-memory fatal error in the script is printed once
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
memory_limit=16M
--FILE--
<?php
class Item {}

/* Handles up to the last of the store's 65536 slots: after the destructors
 * a new object has to double the store. */
$items = [];

do {
    $items[] = $item = new Item;
} while (spl_object_id($item) < 65535);

/* Fills the memory until less than one more block is left. */
$blocks = [];

for (;;) {
    $blocks[] = str_repeat('x', 256 * 1024);
}
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted%s(tried to allocate %d bytes) in %s on line %d
