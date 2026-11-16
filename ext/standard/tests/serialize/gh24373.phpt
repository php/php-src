--TEST--
GH-24373 (serialize() called from a destructor during var_hash destruction reuses the outer var_hash)
--CREDITS--
janhelke
--FILE--
<?php
class D {
    public function __serialize(): array { return []; }
    public function __destruct() {
        $o = new stdClass();
        $GLOBALS['inner'] = serialize([$o, $o]);
    }
}
class Outer {
    public function __serialize(): array { return ["d" => new D()]; }
}
serialize([new Outer(), new stdClass()]);
echo $GLOBALS['inner'], PHP_EOL;

class D2 {
    public function __serialize(): array { return []; }
    public function __destruct() {
        $a = [];
        for ($i = 0; $i < 2000; $i++) { $o = new stdClass(); $a[] = $o; $a[] = $o; }
        serialize($a);
    }
}
class Outer2 {
    public function __serialize(): array { return ["d" => new D2()]; }
}
$shared = [];
for ($i = 0; $i < 200; $i++) { $shared[] = new stdClass(); }
$list = [new Outer2()];
foreach ($shared as $o) { $list[] = $o; $list[] = $o; }
serialize($list);
echo "survived", PHP_EOL;
?>
--EXPECT--
a:2:{i:0;O:8:"stdClass":0:{}i:1;r:2;}
survived
