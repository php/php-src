--TEST--
Readonly property clone indirect variation for JIT
--FILE--
<?php

trait CloneSetOnceTrait {
    public function __clone() {
        $this->prop[] = 1;
    }
}

trait CloneSetTwiceTrait {
    public function __clone() {
        $this->prop[] = 1;
        $this->prop[] = 2;
    }
}

class TestSetOnce {
    use CloneSetOnceTrait;
    public readonly array $prop;
    public function __construct() {
        $this->prop = [];
    }
}

class TestSetTwice {
    use CloneSetTwiceTrait;
    public readonly array $prop;
    public function __construct() {
        $this->prop = [];
    }
}

try {
    var_dump(clone (new TestSetOnce()));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(clone (new TestSetOnce()));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(clone (new TestSetTwice()));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(clone (new TestSetTwice()));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
object(TestSetOnce)#2 (1) {
  ["prop"]=>
  array(1) {
    [0]=>
    int(1)
  }
}
object(TestSetOnce)#1 (1) {
  ["prop"]=>
  array(1) {
    [0]=>
    int(1)
  }
}
Cannot modify readonly property TestSetTwice::$prop
Cannot modify readonly property TestSetTwice::$prop
