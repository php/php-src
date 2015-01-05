--TEST--
ZE2 Callbacks of static functions
--FILE--
<?php
class A {
    public static function who() {
        echo "A\n";
    }
    public static function who2() {
        echo "A\n";
    }
}

class B extends A {
    public static function who() {
        echo "B\n";
    }
}

class C extends B {
    public function call($cb) {
        echo join('|', $cb) . "\n";
        call_user_func($cb);
    }
    public function test() {
        $this->call(array('parent', 'who'));
        $this->call(array('C', 'parent::who'));
        $this->call(array('B', 'parent::who'));
        $this->call(array('E', 'parent::who'));
        $this->call(array('A', 'who'));
        $this->call(array('C', 'who'));
        $this->call(array('B', 'who2'));
    }
}

class D {
    public static function who() {
        echo "D\n";
    }
}

class E extends D {
    public static function who() {
        echo "E\n";
    }
}

$o = new C;
$o->test();

class O {
    public function who() {
        echo "O\n";
    }
}

class P extends O {
    function __toString() {
        return '$this';
    }
    public function who() {
        echo "P\n";
    }
    public function call($cb) {
        echo join('|', $cb) . "\n";
        call_user_func($cb);
    }
    public function test() {
        $this->call(array('parent', 'who'));
        $this->call(array('P', 'parent::who'));
        $this->call(array($this, 'O::who'));
        $this->call(array($this, 'B::who'));
    }
}

echo "===FOREIGN===\n";

$o = new P;
$o->test();

?>
===DONE===
--EXPECTF--
parent|who
B
C|parent::who
B
B|parent::who
A
E|parent::who
D
A|who
A
C|who
B
B|who2
A
===FOREIGN===
parent|who
O
P|parent::who
O
$this|O::who
O
$this|B::who

Warning: call_user_func() expects parameter 1 to be a valid callback, class 'P' is not a subclass of 'B' in %s on line %d
===DONE===
