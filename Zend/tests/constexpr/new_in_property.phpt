--TEST--
Behavior of new in non-static property
--FILE--
<?php

class Test {
    public function __construct() {
        static $i = 0;
        $i++;
        echo "Instantiating object $i\n";
    }
}

class Test2 {
    public $prop = new Test();
}

// Two distinct objects should be created.
$o1 = new Test2;
$o2 = new Test2;
var_dump($o1->prop === $o2->prop);
echo "\n";

class Test3 {
    public function __construct() {
        throw new Exception("Failed to construct");
    }

    public function __destruct() {
        echo "Destructor\n";
    }
}

class Test4 {
    public $prop = new Test3();
    public $prop2 = new Test3();
}

try {
    var_dump(new Test4);
} catch (Exception $e) {
    echo $e, "\n";
}

class Test5 extends DateTime {
    public $prop = new Test3();
    public $prop2 = new Test3();
}

try {
    var_dump(new Test5);
} catch (Exception $e) {
    echo $e, "\n";
}

?>
--EXPECTF--
Instantiating object 1
Instantiating object 2
bool(false)

Exception: Failed to construct in %s:%d
Stack trace:
#0 %s(%d): Test3->__construct()
#1 {main}
Exception: Failed to construct in %s:%d
Stack trace:
#0 %s(%d): Test3->__construct()
#1 {main}
