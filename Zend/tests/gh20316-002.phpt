--TEST--
GH-20316 002: Assign to ref: ref rvalue may be unset
--FILE--
<?php

class C {
    public string $a = '';
    public $b;
    function __toString() {
        unset($GLOBALS['c']);
        unset($GLOBALS['r']);
        return '';
    }
}

for ($i = 0; $i < 2; $i++) {
    $c = new C;
    $r = &$c;
    $c->b = &$c->a;
    $c->b = $c;

    var_dump(isset($c));
}

?>
--EXPECT--
bool(false)
bool(false)
