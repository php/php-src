--TEST--
Bug GH-9801 (Crash when memory limit is exceeded during generator initialization)
--INI--
memory_limit=16m
--SKIPIF--
<?php
$zend_mm_enabled = getenv("USE_ZEND_ALLOC");
if ($zend_mm_enabled === "0") {
    die("skip Zend MM disabled");
}
?>
--FILE--
<?php

function a() {
    yield from a();
}

foreach(a() as $v);
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %s
