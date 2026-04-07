--TEST--
GH-20316 003: Assign to ref: rvalue may be unset
--FILE--
<?php

class C {
    public string $a = '';
    public $b;
    function __toString() {
        unset($GLOBALS['c']);
        return '';
    }
}

for ($i = 0; $i < 2; $i++) {
    $c = new C;
    $c->b = &$c->a;
    $c->b = $c;

    var_dump(isset($c));
}

?>
--EXPECT--
bool(false)
bool(false)
