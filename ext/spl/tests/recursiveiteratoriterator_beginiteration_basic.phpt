--TEST--
SPL: RecursiveIteratorIterator::beginIteration() is called by RecursiveIteratorIterator::rewind()
--CREDITS--
Matt Raines matt@raines.me.uk
#testfest London 2009-05-09
--FILE--
<?php
$sample_array = array(1, 2);
$sub_iterator = new RecursiveArrayIterator($sample_array);

$iterator = new RecursiveIteratorIterator($sub_iterator);
foreach ($iterator as $element) {
  var_dump($element);
}

class SkipsFirstElementRecursiveIteratorIterator extends RecursiveIteratorIterator {
  public function beginIteration() {
    echo "::beginIteration() was invoked\n";
    $this->next();
  }
}
$iterator = new SkipsFirstElementRecursiveIteratorIterator($sub_iterator);
foreach ($iterator as $element) {
  var_dump($element);
}
?>
--EXPECT--
int(1)
int(2)
::beginIteration() was invoked
int(2)
