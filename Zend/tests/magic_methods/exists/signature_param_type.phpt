--TEST--
__exists: signature validation must take a single string param and return bool
--FILE--
<?php

class WrongArgType {
    public function __exists(int $n): bool { return true; }
}
?>
--EXPECTF--
Fatal error: WrongArgType::__exists(): Parameter #1 ($n) must be of type string when declared in %s on line %d
