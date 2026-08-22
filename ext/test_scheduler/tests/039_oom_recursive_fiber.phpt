--TEST--
Out of Memory from recursive fiber creation — under test_scheduler
--INI--
test_scheduler.enable=1
memory_limit=2M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") {
    die("skip Zend MM disabled");
}
?>
--EXTENSIONS--
test_scheduler
--FILE--
<?php

function create_fiber(): Fiber
{
    $fiber = new Fiber('create_fiber');
    $fiber->start();
    return $fiber;
}

$fiber = new Fiber('create_fiber');
$fiber->start();

?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted%s(tried to allocate %d bytes) in %s on line %d
