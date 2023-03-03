--TEST--
__clone() can indirectly modify unlocked readonly properties
--SKIPIF--
<?php
if (function_exists('opcache_get_status') && opcache_get_status()["jit"]["enabled"] ?? false) {
    die('skip Not yet implemented for JIT');
}
?>
--FILE--
<?php

class Foo {
    public function __construct(
        public readonly array $bar
    ) {}

    public function __clone()
    {
        $this->bar['bar'] = 'bar';
    }
}

$foo = new Foo([]);
// First call fills the cache slot
var_dump(clone $foo);
var_dump(clone $foo);

?>
--EXPECTF--
object(Foo)#2 (%d) {
  ["bar"]=>
  array(1) {
    ["bar"]=>
    string(3) "bar"
  }
}
object(Foo)#2 (%d) {
  ["bar"]=>
  array(1) {
    ["bar"]=>
    string(3) "bar"
  }
}
