--TEST--
Bug #40770 (Apache child exits when PHP memory limit reached)
--INI--
memory_limit=8M
--SKIPIF--
<?php
$zend_mm_enabled = getenv("USE_ZEND_ALLOC");
if ($zend_mm_enabled === "0") {
    die("skip Zend MM disabled");
}
?>
--FILE--
<?php
ini_set('display_errors',true);
$mb=148;
$var = '';
for ($i=0; $i<=$mb; $i++) {
        $var.= str_repeat('a',1*1024*1024);
}
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted%s(tried to allocate %d bytes) in %s on line %d
