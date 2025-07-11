--TEST--
Bug #70258 (Segfault if do_resize fails to allocated memory)
--INI--
memory_limit=2M
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    die("xfail fails on Windows Server 2022 and newer.");
}
$zend_mm_enabled = getenv("USE_ZEND_ALLOC");
if ($zend_mm_enabled === "0") {
    die("skip Zend MM disabled");
}
?>
--FILE--
<?php
class A {
    public $arr;
    public function core() {
        $this->arr["no_pack"] = 1;
        while (1) {
            $this->arr[] = 1;
        }
    }
}

$a = new A;
$a->core();
?>
--EXPECTF--
Fatal error: Allowed memory size of 2097152 bytes exhausted%s(tried to allocate %d bytes) in %s on line %d
