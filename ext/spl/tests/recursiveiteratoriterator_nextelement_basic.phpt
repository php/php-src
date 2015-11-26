--TEST--
SPL: RecursiveIteratorIterator::nextElement() is called when the next element is ready
--CREDITS--
Matt Raines matt@raines.me.uk
#testfest London 2009-05-09
--FILE--
<?php
$sample_array = array(1, 2, array(3, 4));
$sub_iterator = new RecursiveArrayIterator($sample_array);

$iterator = new RecursiveIteratorIterator($sub_iterator);
foreach ($iterator as $element) {
  var_dump($element);
}

class NextElementRecursiveIteratorIterator extends RecursiveIteratorIterator {
  public function nextElement() {
    echo "::nextElement() was invoked\n";
  }
}
$iterator = new NextElementRecursiveIteratorIterator($sub_iterator);
foreach ($iterator as $element) {
  var_dump($element);
}
?>
--EXPECT--
int(1)
int(2)
int(3)
int(4)
::nextElement() was invoked
int(1)
::nextElement() was invoked
int(2)
::nextElement() was invoked
int(3)
::nextElement() was invoked
int(4)

