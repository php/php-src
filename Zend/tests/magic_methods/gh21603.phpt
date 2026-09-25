--TEST--
GH-21603: Missing addref for __unset
--CREDITS--
cnwangjihe
--FILE--
<?php

class C {
    public function __unset($name) {
        global $c;
        $c = null;
        var_dump($this);
    }
}

$c = new C;
unset($c->prop);

?>
--EXPECTF--
object(C)#%d (0) {
}
