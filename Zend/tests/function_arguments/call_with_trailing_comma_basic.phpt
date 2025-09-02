--TEST--
Allow trailing commas in function and method calls
--FILE--
<?php
function foo(...$args) {
  echo __FUNCTION__ . "\n";
  var_dump($args);
}
foo(
  'function',
  'bar',
);

class Foo
{
  public function __construct(...$args) {
    echo __FUNCTION__ . "\n";
    var_dump($args);
  }

  public function bar(...$args) {
    echo __FUNCTION__ . "\n";
    var_dump($args);
  }

  public function __invoke(...$args) {
    echo __FUNCTION__ . "\n";
    var_dump($args);
  }
}

$foo = new Foo(
  'constructor',
  'bar',
);

$foo->bar(
  'method',
  'bar',
);

$foo(
  'invoke',
  'bar',
);

$bar = function(...$args) {
  echo __FUNCTION__ . "\n";
  var_dump($args);
};

$bar(
  'closure',
  'bar',
);

# Make sure to hit the "not really a function" language constructs
unset($foo, $bar,);
var_dump(isset($foo, $bar,));
?>
--EXPECTF--
foo
array(2) {
  [0]=>
  string(8) "function"
  [1]=>
  string(3) "bar"
}
__construct
array(2) {
  [0]=>
  string(11) "constructor"
  [1]=>
  string(3) "bar"
}
bar
array(2) {
  [0]=>
  string(6) "method"
  [1]=>
  string(3) "bar"
}
__invoke
array(2) {
  [0]=>
  string(6) "invoke"
  [1]=>
  string(3) "bar"
}
{closure:%s:%d}
array(2) {
  [0]=>
  string(7) "closure"
  [1]=>
  string(3) "bar"
}
bool(false)
