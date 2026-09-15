--TEST--
Bug #76846: Segfault in shutdown function after memory limit error
--INI--
memory_limit=33M
--SKIPIF--
<?php
$zend_mm_enabled = getenv("USE_ZEND_ALLOC");
if ($zend_mm_enabled === "0") {
    die("skip Zend MM disabled");
}
?>
--FILE--
<?php

register_shutdown_function(function() {
    new stdClass;
});

$ary = [];
while (true) {
    $ary[] = new stdClass;
}

?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted%s(tried to allocate %d bytes) in %s on line %d%A
