--TEST--
Bug #25359 (array_multisort() does not work in a function if array is global or reference)
--FILE--
<?php

function does_not_work()
{
    global $data; // Remove this line to make array_multisort() work

    $data = array('first', 'fifth', 'second', 'forth', 'third');
    $sort = array(1, 5, 2, 4, 3);
    array_multisort($sort, $data);

    var_dump($data);
}

does_not_work();

?>
--EXPECT--
array(5) {
  [0]=>
  string(5) "first"
  [1]=>
  string(6) "second"
  [2]=>
  string(5) "third"
  [3]=>
  string(5) "forth"
  [4]=>
  string(5) "fifth"
}
