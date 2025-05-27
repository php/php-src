--TEST--
Bug #79836 (use-after-free in concat_function)
--INI--
memory_limit=10M
--FILE--
<?php
class Foo {
    public function __toString() {
        return str_repeat('a', 10);
    }
}

$i = str_repeat('a', 5 * 1024 * 1024);
$e = new Foo();
$e .= $i;
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted%s(tried to allocate %d bytes) in %s on line %d
