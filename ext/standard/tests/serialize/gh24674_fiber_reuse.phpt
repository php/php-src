--TEST--
GH-24674 (A fiber reusing a freed fiber's context does not see its unserialize context)
--INI--
memory_limit=8M
--FILE--
<?php
class S implements Serializable {
    public function serialize() { return ''; }
    public function unserialize($d) { Fiber::suspend(); }
    public function __serialize(): array { return []; }
    public function __unserialize(array $d): void {}
}

$a = new Fiber(function () {
    unserialize('a:2:{i:0;s:1:"A";i:1;C:1:"S":1:{A}}');
});
$a->start();
register_shutdown_function(function () {
    global $a;
    $a = null;
    $b = new Fiber(function () {
        var_dump(unserialize('R:1;'));
    });
    $b->start();
});
str_repeat('x', 16 * 1024 * 1024);
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted%s(tried to allocate %d bytes) in %s on line %d

Warning: unserialize(): Error at offset 4 of 4 bytes in %s on line %d
bool(false)
