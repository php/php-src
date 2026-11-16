--TEST--
Bug #76846: Segfault in shutdown function after memory limit error
--INI--
memory_limit=33M
zend.enable_gc=0
--SKIPIF--
<?php
$zend_mm_enabled = getenv("USE_ZEND_ALLOC");
if ($zend_mm_enabled === "0") {
    die("skip Zend MM disabled");
}
?>
--FILE--
<?php

class C {
    public $c;
    public function __construct() {
        $this->c = $this;
    }
}
register_shutdown_function(function() {
    new C;
});

while (true) {
    new C;
}

?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted%s(tried to allocate %d bytes) in %s on line %d%A
