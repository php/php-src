--TEST--
Bug #76047: Use-after-free when accessing already destructed backtrace arguments
--FILE--
<?php

class Vuln {
    public $a;
    public function __destruct() {
        unset($this->a);
        $backtrace = (new Exception)->getTrace();
        var_dump($backtrace);
    }
}

function test($arg) {
    $arg = str_shuffle(str_repeat('A', 79));
    $vuln = new Vuln();
    $vuln->a = $arg;
}

function test2($arg) {
    $$arg = 1; // Trigger symbol table
    $arg = str_shuffle(str_repeat('A', 79));
    $vuln = new Vuln();
    $vuln->a = $arg;
}

test('x');
test2('x');

?>
--EXPECTF--
array(1) {
  [0]=>
  array(6) {
    ["file"]=>
    string(%d) "%s"
    ["line"]=>
    int(%d)
    ["function"]=>
    string(10) "__destruct"
    ["class"]=>
    string(4) "Vuln"
    ["type"]=>
    string(2) "->"
    ["args"]=>
    array(0) {
    }
  }
}
array(1) {
  [0]=>
  array(6) {
    ["file"]=>
    string(%d) "%s"
    ["line"]=>
    int(%d)
    ["function"]=>
    string(10) "__destruct"
    ["class"]=>
    string(4) "Vuln"
    ["type"]=>
    string(2) "->"
    ["args"]=>
    array(0) {
    }
  }
}
