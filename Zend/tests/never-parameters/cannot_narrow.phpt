--TEST--
`never` parameter types - `never` narrows the signature
--FILE--
<?php

interface Base {
    public function example(string $v);
}

interface WithNever extends Base {
    public function example(never $v);
}

?>
--EXPECTF--
Fatal error: Declaration of WithNever::example(never $v) must be compatible with Base::example(string $v) in %s on line %d
