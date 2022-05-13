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
        try {
            $this->call(array($this, 'B::who'));
        } catch (TypeError $e) {
            echo $e->getMessage(), "\n";
        }
    }
}

echo "===FOREIGN===\n";

$o = new P;
$o->test();

?>
--EXPECTF--
parent|who

Deprecated: Use of "parent" in callables is deprecated in %s on line %d
B
C|parent::who

Deprecated: Callables of the form ["C", "parent::who"] are deprecated in %s on line %d
B
B|parent::who

Deprecated: Callables of the form ["B", "parent::who"] are deprecated in %s on line %d
A
E|parent::who

Deprecated: Callables of the form ["E", "parent::who"] are deprecated in %s on line %d
D
A|who
A
C|who
B
B|who2
A
===FOREIGN===
parent|who

Deprecated: Use of "parent" in callables is deprecated in %s on line %d
O
P|parent::who

Deprecated: Callables of the form ["P", "parent::who"] are deprecated in %s on line %d
O
$this|O::who

Deprecated: Callables of the form ["P", "O::who"] are deprecated in %s on line %d
O
$this|B::who
call_user_func(): Argument #1 ($callback) must be a valid callback, class P is not a subclass of B
