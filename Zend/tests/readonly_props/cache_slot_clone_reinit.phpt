--TEST--
Readonly: cache slot primed by a failed modification must not skip the set visibility check on reinitialization
--FILE--
<?php
class P {
    public readonly int $x;
    public function __construct() { $this->x = 1; }
    public function __clone() { w($this, 3); }
}
function w(P $p, int $v) {
    try {
        $p->x = $v;   // same opline every call
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}
$p = new P();
w($p, 2);        // readonly modification error, cache slot stays primed
$q = clone $p;   // __clone -> w(): global scope must not be able to reinitialize
var_dump($q->x);

?>
--EXPECT--
Cannot modify readonly property P::$x
Cannot modify protected(set) readonly property P::$x from global scope
int(1)
