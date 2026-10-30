--TEST--
GH-21605: Missing addref for Countable::count()
--CREDITS--
cnwangjihe
--FILE--
<?php

class C implements Countable {
    public function count(): int {
        global $c;
        $c = null;
        var_dump($this);
        return 42;
    }
}

$c = new C;
var_dump(count($c));

?>
--EXPECTF--
object(C)#%d (0) {
}
int(42)
