--TEST--
protected returns
--FILE--
<?php
class Outer {
    protected class Inner {
        public function __construct() {
            var_dump($this);
        }
    }

    public function test(): Outer::Inner {
        return new Outer::Inner;
    }
}
$inner = new Outer()->test();

class Outer2 {
    public function test(Outer::Inner $wee) {
         var_dump($wee);
    }
}

new Outer2()->test($inner);
?>
--EXPECTF--
object(Outer::Inner)#2 (0) {
}

Fatal error: Protected inner class Outer::Inner cannot be used in the scope of Outer2 in %s on line %d
