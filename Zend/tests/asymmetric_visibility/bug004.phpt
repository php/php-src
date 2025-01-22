--TEST--
Missing property initialization for private(set) constructor promoted property
--FILE--
<?php

class T {
    public function __construct(
        private(set) string $prop,
    ) {}
}
var_dump(new T('Test'));

?>
--EXPECTF--
object(T)#%d (1) {
  ["prop"]=>
  string(4) "Test"
}
