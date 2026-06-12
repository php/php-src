--TEST--
__exists: return type must be bool
--FILE--
<?php

class WrongReturnType {
    public function __exists(string $n): string { return ''; }
}
?>
--EXPECTF--
Fatal error: WrongReturnType::__exists(): Return type must be bool when declared in %s on line %d
