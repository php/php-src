--TEST--
__exists: recursion guard prevents infinite recursion when __exists checks the same property
--FILE--
<?php

class C {
    public function __exists(string $n): bool {
        echo "  __exists($n) entry\n";
        // Recursive isset on the same property must short-circuit to avoid loop.
        $r = isset($this->$n);
        echo "  __exists($n) inner isset = ", ($r ? 'true' : 'false'), "\n";
        return false;
    }
    public function __get(string $n): mixed {
        echo "  __get($n): should not be called when __exists returns false\n";
        return null;
    }
}

$c = new C;
var_dump($c->any ?? 'fb');

?>
--EXPECT--
  __exists(any) entry
  __exists(any) inner isset = false
string(2) "fb"
