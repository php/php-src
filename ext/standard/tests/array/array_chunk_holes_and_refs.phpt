--TEST--
array_chunk() on arrays with holes and on referenced values
--FILE--
<?php
// Packed array that keeps IS_UNDEF holes after unset()
$a = [0, 1, 2, 3, 4, 5, 6, 7];
unset($a[0], $a[3], $a[7]);
var_dump(array_chunk($a, 2));
var_dump(array_chunk($a, 2, true));

// String-keyed hash with holes: chunks are compacted lists
$h = ['a' => 1, 'b' => 2, 'c' => 3, 'd' => 4, 'e' => 5];
unset($h['b'], $h['e']);
var_dump(array_chunk($h, 2));
var_dump(array_chunk($h, 2, true));

// Last chunk shorter than size
var_dump(array_chunk([1, 2, 3, 4, 5], 3));

// Chunk size larger than the array
var_dump(array_chunk([1, 2], 10));

// A singly-referenced value is unwrapped like a plain copy:
// later writes to the input must not leak into the chunks
$b = [1, 2, 3];
$r = &$b[1];
unset($r);
$c = array_chunk($b, 2);
$b[1] = 99;
var_dump($c);

// Unwrapped refcounted payloads (object, array, string) must be retained by
// the chunk: the input is released first, and the destructor must run only
// once the chunk is released too
class Dtor {
    public function __destruct() { echo "Dtor destroyed\n"; }
}
$e = [new Dtor, range(1, 2), str_repeat('s', 3)];
foreach ($e as &$v) {}
unset($v);
$c = array_chunk($e, 2);
unset($e);
echo "input released\n";
var_dump($c);
unset($c);
echo "chunks released\n";

// Live references are preserved and kept alive by the chunk
$d = [1, 2];
$live = &$d[0];
$c = array_chunk($d, 2);
unset($d);
$live = 42;
var_dump($c);
unset($live);
?>
--EXPECT--
array(3) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [1]=>
  array(2) {
    [0]=>
    int(4)
    [1]=>
    int(5)
  }
  [2]=>
  array(1) {
    [0]=>
    int(6)
  }
}
array(3) {
  [0]=>
  array(2) {
    [1]=>
    int(1)
    [2]=>
    int(2)
  }
  [1]=>
  array(2) {
    [4]=>
    int(4)
    [5]=>
    int(5)
  }
  [2]=>
  array(1) {
    [6]=>
    int(6)
  }
}
array(2) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(3)
  }
  [1]=>
  array(1) {
    [0]=>
    int(4)
  }
}
array(2) {
  [0]=>
  array(2) {
    ["a"]=>
    int(1)
    ["c"]=>
    int(3)
  }
  [1]=>
  array(1) {
    ["d"]=>
    int(4)
  }
}
array(2) {
  [0]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  [1]=>
  array(2) {
    [0]=>
    int(4)
    [1]=>
    int(5)
  }
}
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}
array(2) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [1]=>
  array(1) {
    [0]=>
    int(3)
  }
}
input released
array(2) {
  [0]=>
  array(2) {
    [0]=>
    object(Dtor)#1 (0) {
    }
    [1]=>
    array(2) {
      [0]=>
      int(1)
      [1]=>
      int(2)
    }
  }
  [1]=>
  array(1) {
    [0]=>
    string(3) "sss"
  }
}
Dtor destroyed
chunks released
array(1) {
  [0]=>
  array(2) {
    [0]=>
    &int(42)
    [1]=>
    int(2)
  }
}
