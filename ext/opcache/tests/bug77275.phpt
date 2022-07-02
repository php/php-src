--TEST--
Bug #77275: OPcache optimization problem for ArrayAccess->offsetGet(string)
--INI--
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
namespace Foo;
class Bar { public function get() {} }
class Record implements \ArrayAccess {
    public function offsetSet($offset, $value): void { throw new \Exception; }
    public function offsetGet($offset): mixed { var_dump($offset); return null; }
    public function offsetExists($offset): bool { throw new \Exception; }
    public function offsetUnset($offset): void { throw new \Exception; }
}
class Baz {
    public function run() {
        $a = pow(1, 2);
        $b = new Bar();
        $c = new Bar();
        $d = new Bar();
        $id = $b->get('a', 'b', 'c');
        $rec = new Record();
        $id = $rec['a'];
    }
}
(new Baz())->run();
?>
--EXPECT--
string(1) "a"
