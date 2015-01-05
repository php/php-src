--TEST--
SPL: Attempt to corrupt the heap while iterating
--CREDITS--
Lukasz Andrzejak meltir@meltir.com
#testfest London 2009-05-09
--FILE--
<?php
class ext_heap extends SplMaxHeap {
  public $fail = false;
  public function compare($val1,$val2) {
    if ($this->fail)
      throw new Exception('Corrupting heap',99);
    return 0;
  }
}

$h = new ext_heap();
$h->insert(array('foobar'));
$h->insert(array('foobar1'));
$h->insert(array('foobar2'));

try {
  $h->fail=true;
  foreach ($h as $value) {};
  echo "I should have raised an exception here";
} catch (Exception $e) {
  if ($e->getCode()!=99) echo "Unexpected exception";
}

var_dump($h);
?>
--EXPECTF--
object(ext_heap)#%d (4) {
  [%u|b%"fail"]=>
  bool(true)
  [%u|b%"flags":%u|b%"SplHeap":private]=>
  int(0)
  [%u|b%"isCorrupted":%u|b%"SplHeap":private]=>
  bool(true)
  [%u|b%"heap":%u|b%"SplHeap":private]=>
  array(2) {
    [0]=>
    array(1) {
      [0]=>
      %unicode|string%(7) "foobar2"
    }
    [1]=>
    array(1) {
      [0]=>
      %unicode|string%(7) "foobar1"
    }
  }
}
