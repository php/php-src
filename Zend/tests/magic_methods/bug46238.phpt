--TEST--
Bug #46238 (Segmentation fault on static call with empty string method)
--FILE--
<?php

class a {
        static function __callStatic($name, $arguments)
        {
                var_dump(array($name, $arguments));
        }
}

$a = 'a';
$b = '';

$a::$b($a);
$a::$b(array());
$a::$b(NULL);
$a::$b(1);
$a::$b();


$b = "\0";

$a::$b($a);
$a::$b(array());
$a::$b(NULL);
$a::$b(1);
$a::$b();

?>
--EXPECT--
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  array(1) {
    [0]=>
    string(1) "a"
  }
}
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  array(1) {
    [0]=>
    array(0) {
    }
  }
}
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  array(1) {
    [0]=>
    NULL
  }
}
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  array(1) {
    [0]=>
    int(1)
  }
}
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  array(0) {
  }
}
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  array(1) {
    [0]=>
    string(1) "a"
  }
}
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  array(1) {
    [0]=>
    array(0) {
    }
  }
}
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  array(1) {
    [0]=>
    NULL
  }
}
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  array(1) {
    [0]=>
    int(1)
  }
}
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  array(0) {
  }
}
