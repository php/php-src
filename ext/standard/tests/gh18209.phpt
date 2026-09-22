--TEST--
GH-18209: Use-after-free in extract() with EXTR_REFS
--CREDITS--
Noam Rathaus (nrathaus)
--FILE--
<?php

class C {
    public function __destruct() {
        var_dump($GLOBALS['b']);
        $GLOBALS['b'] = 43;
    }
}

$b = new C;
$array = ['b' => 42];
extract($array, EXTR_REFS);
var_dump($b);

?>
--EXPECT--
int(42)
int(43)
