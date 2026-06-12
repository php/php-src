--TEST--
__exists: return type must be declared as bool (no BC carve-out for new method)
--FILE--
<?php

class UntypedReturn {
    public function __exists(string $n) { return true; }
}
?>
--EXPECTF--
Fatal error: UntypedReturn::__exists(): Return type must be bool in %s on line %d
