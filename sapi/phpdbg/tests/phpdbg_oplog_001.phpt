--TEST--
Test phpdbg_*_oplog() functions
--PHPDBG--
r
q
--EXPECTF--
[Successful compilation of %s]
prompt> halloarray(2) {
  ["%s"]=>
  array(6) {
    [11]=>
    int(0)
    [13]=>
    int(1)
    [17]=>
    int(2)
    [18]=>
    int(2)
    [19]=>
    int(3)
    [21]=>
    int(4)
  }
  ["A::b"]=>
  array(2) {
    [5]=>
    int(5)
    [4]=>
    int(1)
  }
}
[Script ended normally]
prompt> 
--FILE--
<?php

class A {
  public function b($c = 1) {
    if ($c == 1) {
      // comment
    }
  }
}

phpdbg_start_oplog();

echo "hallo";

// fcalls

$a = new A();
$a->b();
$a->b('ha');

var_dump(phpdbg_end_oplog(["functions" => true, "show_unexecuted" => true]));

