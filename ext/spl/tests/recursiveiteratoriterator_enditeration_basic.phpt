--TEST--
SPL: RecursiveIteratorIterator::endIteration() is called when ::valid() first returns false
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

class EndIterationRecursiveIteratorIterator extends RecursiveIteratorIterator {
  public function endIteration() {
    echo "::endIteration() was invoked\n";
  }
}
$iterator = new EndIterationRecursiveIteratorIterator($sub_iterator);
foreach ($iterator as $element) {
  var_dump($element);
}
?>
--EXPECT--
int(1)
int(2)
int(1)
int(2)
::endIteration() was invoked

