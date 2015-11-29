--TEST--
SPL: AppendIterator and its ArrayIterator
--FILE--
<?php

function test_error_handler($errno, $msg, $filename, $linenum)
{
	echo "Error $msg in $filename on line $linenum\n";
	return true;
}

set_error_handler('test_error_handler');

$it = new AppendIterator;

try {
	$it->append(array());
} catch (Error $e) {
	test_error_handler($e->getCode(), $e->getMessage(), $e->getFile(), $e->getLine());
}
$it->append(new ArrayIterator(array(1)));
$it->append(new ArrayIterator(array(21, 22)));

var_dump($it->getArrayIterator());

$it->append(new ArrayIterator(array(31, 32, 33)));

var_dump($it->getArrayIterator());

$idx = 0;

foreach($it as $k => $v)
{
	echo '===' . $idx++ . "===\n";
	var_dump($it->getIteratorIndex());
	var_dump($k);
	var_dump($v);
}

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Error Argument 1 passed to AppendIterator::append() must implement interface Iterator, array given in %siterator_042.php on line %d
object(ArrayIterator)#%d (1) {
  %s"storage"%s"ArrayIterator":private]=>
  array(2) {
    [0]=>
    object(ArrayIterator)#%d (1) {
      %s"storage"%s"ArrayIterator":private]=>
      array(1) {
        [0]=>
        int(1)
      }
    }
    [1]=>
    object(ArrayIterator)#%d (1) {
      %s"storage"%s"ArrayIterator":private]=>
      array(2) {
        [0]=>
        int(21)
        [1]=>
        int(22)
      }
    }
  }
}
object(ArrayIterator)#%d (1) {
  %s"storage"%s"ArrayIterator":private]=>
  array(3) {
    [0]=>
    object(ArrayIterator)#%d (1) {
      %s"storage"%s"ArrayIterator":private]=>
      array(1) {
        [0]=>
        int(1)
      }
    }
    [1]=>
    object(ArrayIterator)#%d (1) {
      %s"storage"%s"ArrayIterator":private]=>
      array(2) {
        [0]=>
        int(21)
        [1]=>
        int(22)
      }
    }
    [2]=>
    object(ArrayIterator)#%d (1) {
      %s"storage"%s"ArrayIterator":private]=>
      array(3) {
        [0]=>
        int(31)
        [1]=>
        int(32)
        [2]=>
        int(33)
      }
    }
  }
}
===0===
int(0)
int(0)
int(1)
===1===
int(1)
int(0)
int(21)
===2===
int(1)
int(1)
int(22)
===3===
int(2)
int(0)
int(31)
===4===
int(2)
int(1)
int(32)
===5===
int(2)
int(2)
int(33)
===DONE===
