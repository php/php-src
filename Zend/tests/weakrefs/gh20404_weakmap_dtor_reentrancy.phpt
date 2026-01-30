--TEST--
GH-20404: Crash when adding object to WeakMap during destruction
--FILE--
<?php

$w = new WeakMap;

$o = new stdClass;
$w[$o] = new class($r) {
    function __construct(public &$r) {}
    function __destruct() {
        global $refs;
        $o = $this->r->get();
        for ($i = 0; $i < 8; ++$i) {
            $r = new WeakMap;
            $r[$o] = 1;
            $refs[] = $r;
        }
    }
};
$r = WeakReference::create($o);

echo "END OF SCRIPT\n";
?>
--EXPECT--
END OF SCRIPT