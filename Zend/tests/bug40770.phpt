--TEST--
Bug #40770 (Apache child exits when PHP memory limit reached)
--INI--
memory_limit=8M
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows' && version_compare(PHP_VERSION, '8.4', '<')) {
    die("xfail fails on Windows Server 2022 and newer.");
}
$zend_mm_enabled = getenv("USE_ZEND_ALLOC");
if ($zend_mm_enabled === "0") {
    die("skip Zend MM disabled");
}
?>
--FILE--
<?php
ini_set('display_errors',true);
$var = 'a';
while (true) {
        $var .= $var;
}
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted%s(tried to allocate %d bytes) in %s on line %d
