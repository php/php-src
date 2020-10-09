--TEST--
Ensure foreach splits the iterated entity from its cow reference set, for all sorts of iterated entities.
--FILE--
<?php

  echo "\n" . '$a' . "\n";
  $b = $a = array('original');
  foreach($a as $k=>&$v) {
     $v = 'changed';
  }
  var_dump($b);
  unset($a, $b);

  echo "\n" . '${\'a\'}' . "\n";
  $b = $a = array('original');
  foreach(${'a'} as $k=>&$v) {
     $v = 'changed';
  }
  var_dump($b);
  unset($a, $b);

  echo "\n" . '$$a' . "\n";
  $a = 'blah';
  $$a = array('original');
  $b = $$a;
  foreach($$a as $k=>&$v) {
     $v = 'changed';
  }
  var_dump($b);
  unset($a, $b);

  echo "\n" . '$a[0]' . "\n";
  $b = $a[0] = array('original');
  foreach($a[0] as $k=>&$v) {
     $v = 'changed';
  }
  var_dump($b);
  unset($a, $b);

  echo "\n" . '$a[0][0]' . "\n";
  $b = $a[0][0] = array('original');
  foreach($a[0][0] as $k=>&$v) {
     $v = 'changed';
  }
  var_dump($b);
  unset($a, $b);

  echo "\n" . '$a->b' . "\n";
  $a = new stdClass;
  $b = $a->b = array('original');
  foreach($a->b as $k=>&$v) {
     $v = 'changed';
  }
  var_dump($b);
  unset($a, $b);

  echo "\n" . '$a->b->c' . "\n";
  $a = new stdClass;
  $a->b = new stdClass;
  $b = $a->b->c = array('original');
  foreach($a->b as $k=>&$v) {
     $v = 'changed';
  }
  var_dump($b);
  unset($a, $b);

  echo "\n" . '$a->b[0]' . "\n";
  $a = new stdClass;
  $b = $a->b[0] = array('original');
  foreach($a->b[0] as $k=>&$v) {
     $v = 'changed';
  }
  var_dump($b);
  unset($a, $b);

  echo "\n" . '$a->b[0][0]' . "\n";
  $a = new stdClass;
  $b = $a->b[0][0] = array('original');
  foreach($a->b[0][0] as $k=>&$v) {
     $v = 'changed';
  }
  var_dump($b);
  unset($a, $b);

  echo "\n" . '$a->b[0]->c' . "\n";
  $a = new stdClass;
  $a->b[0] = new stdClass;
  $b = $a->b[0]->c = array('original');
  foreach($a->b[0]->c as $k=>&$v) {
     $v = 'changed';
  }
  var_dump($b);
  unset($a, $b);

  class C {
    public static $a;
  }

  echo "\n" . 'C::$a' . "\n";
  C::$a = array('original');
  $b = C::$a;
  foreach(C::$a as $k=>&$v) {
     $v = 'changed';
  }
  var_dump($b);
  unset($a, $b);

  echo "\n" . 'C::$a[0]' . "\n";
  C::$a[0] = array('original');
  $b = C::$a[0];
  foreach(C::$a[0] as $k=>&$v) {
     $v = 'changed';
  }
  var_dump($b);
  unset(C::$a[0], $b);

  echo "\n" . 'C::$a[0]->b' . "\n";
  C::$a[0] = new stdClass;
  C::$a[0]->b = array('original');
  $b = C::$a[0]->b;
  foreach(C::$a[0]->b as $k=>&$v) {
     $v = 'changed';
  }
  var_dump($b);
  unset(C::$a[0]->b, $b);
?>
--EXPECT--
$a
array(1) {
  [0]=>
  string(8) "original"
}

${'a'}
array(1) {
  [0]=>
  string(8) "original"
}

$$a
array(1) {
  [0]=>
  string(8) "original"
}

$a[0]
array(1) {
  [0]=>
  string(8) "original"
}

$a[0][0]
array(1) {
  [0]=>
  string(8) "original"
}

$a->b
array(1) {
  [0]=>
  string(8) "original"
}

$a->b->c
array(1) {
  [0]=>
  string(8) "original"
}

$a->b[0]
array(1) {
  [0]=>
  string(8) "original"
}

$a->b[0][0]
array(1) {
  [0]=>
  string(8) "original"
}

$a->b[0]->c
array(1) {
  [0]=>
  string(8) "original"
}

C::$a
array(1) {
  [0]=>
  string(8) "original"
}

C::$a[0]
array(1) {
  [0]=>
  string(8) "original"
}

C::$a[0]->b
array(1) {
  [0]=>
  string(8) "original"
}
