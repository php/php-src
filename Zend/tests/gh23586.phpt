--TEST--
GH-23586 (GC traversal stack allocation must not exhaust the memory limit)
--SKIPIF--
<?php
if (getenv('USE_ZEND_ALLOC') === '0') {
    die('skip Zend MM required');
}
?>
--INI--
memory_limit=93M
--FILE--
<?php
class Node {
    public $children = [];

    public function __construct(?Node $parent = null) {
        if ($parent) {
            $parent->children[] = $this;
        }
        $this->children[] = $this;
    }

    public function __destruct() {
        $this->children = null;
    }
}

for ($n = 0; $n < 2; $n++) {
    $top = new Node();
    for ($i = 0; $i < 16; $i++) {
        $ci = new Node($top);
        for ($j = 0; $j < 16; $j++) {
            $cj = new Node($ci);
            while ($n) {
                $ck = new Node($cj);
            }
        }
    }
}
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted%S (tried to allocate %d bytes) in %s on line %d
