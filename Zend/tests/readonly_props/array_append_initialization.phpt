--TEST--
Use array append as initialization
--FILE--
<?php

class C {
    public readonly array $a;

    public function init() {
        $this->a[] = 1;
        var_dump($this->a);
    }
}


function init() {
    $c = new C;
    $c->a[] = 1;
    var_dump($c->a);
}

(new C)->init();

try {
    init();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
Cannot initialize readonly property C::$a from global scope
