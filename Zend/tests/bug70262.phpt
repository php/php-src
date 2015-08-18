--TEST--
Bug #70262 (Accessing array crashes)
--FILE--
<?php
class C {
    public $arguments;
    public function __construct($arg) {
        $this->arguments = $arg;
    }
}

function & a(&$arg) {
    $c = new C($arg);
    $arg[] = $c;
    return $c;
}

function c($arr) {
    a($arr)->arguments[0] = "bad";
}

$arr = array();
$arr[] = "foo";
$arr[] = "bar";
c($arr);
var_dump($arr);
?>
--EXPECT--
array(2) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(3) "bar"
}
