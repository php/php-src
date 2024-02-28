--TEST--
Bug #52193 (converting closure to array yields empty array)
--FILE--
<?php

var_dump((array) 1);
var_dump((array) NULL);
var_dump((array) new stdclass);
var_dump($h = (array) function () { return 2; });
var_dump($h[0]());

$i = function () use (&$h) {
    return $h;
};

var_dump($x = (array)$i);
var_dump($y = $x[0]);
var_dump($y());

$items = range(1, 5);
$func = function(){ return 'just a test'; };

array_splice($items, 0 , 4, $func);
var_dump($items);

?>
--EXPECTF--
array(1) {
  [0]=>
  int(1)
}
array(0) {
}
array(0) {
}
array(1) {
  [0]=>
  object(Closure)#%d (1) {
    ["name"]=>
    string(%d) "{closure:%s:%d}"
  }
}
int(2)
array(1) {
  [0]=>
  object(Closure)#%d (2) {
    ["name"]=>
    string(%d) "{closure:%s:%d}"
    ["static"]=>
    array(1) {
      ["h"]=>
      &array(1) {
        [0]=>
        object(Closure)#%d (1) {
          ["name"]=>
          string(%d) "{closure:%s:%d}"
        }
      }
    }
  }
}
object(Closure)#%d (2) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
  ["static"]=>
  array(1) {
    ["h"]=>
    &array(1) {
      [0]=>
      object(Closure)#%d (1) {
        ["name"]=>
        string(%d) "{closure:%s:%d}"
      }
    }
  }
}
array(1) {
  [0]=>
  object(Closure)#%d (1) {
    ["name"]=>
    string(%d) "{closure:%s:%d}"
  }
}
array(2) {
  [0]=>
  object(Closure)#%d (1) {
    ["name"]=>
    string(%d) "{closure:%s:%d}"
  }
  [1]=>
  int(5)
}
