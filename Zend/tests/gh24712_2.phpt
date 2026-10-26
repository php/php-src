--TEST--
GH-24712 (Use-after-free in static variable binding when destructor modifies the variable)
--FILE--
<?php
class C {
    function __construct(public string $var) {}
    function __destruct() {
        $GLOBALS[$this->var] = null;
    }
}
function init() {
    return 1;
}
$a = new C('a');
static $a;
var_dump($a);

for ($i = 0; $i < 2; $i++) {
    unset($b);
    $b = $i ? new C('b') : 0;
    static $b = init();
}
var_dump($b);
?>
--EXPECT--
NULL
NULL
