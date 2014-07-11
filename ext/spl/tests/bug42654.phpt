--TEST--
Bug #42654 (RecursiveIteratorIterator modifies only part of leaves)
--FILE--
<?php
$data = array(1 => 'val1',
              array(2 => 'val2',
                    array(3 => 'val3'),
                   ),
              4 => 'val4'
             );

$iterator = new RecursiveIteratorIterator(new
RecursiveArrayIterator($data));
foreach($iterator as $foo) {
    $key = $iterator->key();
    echo "update $key\n";
    var_dump($iterator->getInnerIterator());
    $iterator->offsetSet($key, 'alter');
    var_dump($iterator->getInnerIterator());
}
$copy = $iterator->getArrayCopy();
var_dump($copy);
?>
--EXPECTF--
update 1
object(RecursiveArrayIterator)#%d (1) {
  ["storage":"ArrayIterator":private]=>
  array(3) {
    [1]=>
    string(4) "val1"
    [2]=>
    array(2) {
      [2]=>
      string(4) "val2"
      [3]=>
      array(1) {
        [3]=>
        string(4) "val3"
      }
    }
    [4]=>
    string(4) "val4"
  }
}
object(RecursiveArrayIterator)#%d (1) {
  ["storage":"ArrayIterator":private]=>
  array(3) {
    [1]=>
    string(5) "alter"
    [2]=>
    array(2) {
      [2]=>
      string(4) "val2"
      [3]=>
      array(1) {
        [3]=>
        string(4) "val3"
      }
    }
    [4]=>
    string(4) "val4"
  }
}
update 2
object(RecursiveArrayIterator)#%d (1) {
  ["storage":"ArrayIterator":private]=>
  array(2) {
    [2]=>
    string(4) "val2"
    [3]=>
    array(1) {
      [3]=>
      string(4) "val3"
    }
  }
}
object(RecursiveArrayIterator)#%d (1) {
  ["storage":"ArrayIterator":private]=>
  array(2) {
    [2]=>
    string(5) "alter"
    [3]=>
    array(1) {
      [3]=>
      string(4) "val3"
    }
  }
}
update 3
object(RecursiveArrayIterator)#%d (1) {
  ["storage":"ArrayIterator":private]=>
  array(1) {
    [3]=>
    string(4) "val3"
  }
}
object(RecursiveArrayIterator)#%d (1) {
  ["storage":"ArrayIterator":private]=>
  array(1) {
    [3]=>
    string(5) "alter"
  }
}
update 4
object(RecursiveArrayIterator)#%d (1) {
  ["storage":"ArrayIterator":private]=>
  array(3) {
    [1]=>
    string(5) "alter"
    [2]=>
    array(2) {
      [2]=>
      string(4) "val2"
      [3]=>
      array(1) {
        [3]=>
        string(4) "val3"
      }
    }
    [4]=>
    string(4) "val4"
  }
}
object(RecursiveArrayIterator)#%d (1) {
  ["storage":"ArrayIterator":private]=>
  array(3) {
    [1]=>
    string(5) "alter"
    [2]=>
    array(2) {
      [2]=>
      string(4) "val2"
      [3]=>
      array(1) {
        [3]=>
        string(4) "val3"
      }
    }
    [4]=>
    string(5) "alter"
  }
}
array(3) {
  [1]=>
  string(5) "alter"
  [2]=>
  array(2) {
    [2]=>
    string(4) "val2"
    [3]=>
    array(1) {
      [3]=>
      string(4) "val3"
    }
  }
  [4]=>
  string(5) "alter"
}
