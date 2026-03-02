--TEST--
GH-11972 (RecursiveCallbackFilterIterator regression in 8.1.18)
--EXTENSIONS--
spl
--FILE--
<?php

class RecursiveFilterTest {
    public function traverse(array $variables): array {
        $array_iterator = new \RecursiveArrayIterator($variables);
        $filter_iterator = new \RecursiveCallbackFilterIterator($array_iterator, [
            $this, 'isCyclic',
        ]);
        $recursive_iterator = new \RecursiveIteratorIterator($filter_iterator, \RecursiveIteratorIterator::SELF_FIRST);
        $recursive_iterator->setMaxDepth(20);
        foreach ($recursive_iterator as $value) {
            // Avoid recursion by marking where we've been.
            $value['#override_mode_breadcrumb'] = true;
        }
        return \iterator_to_array($recursive_iterator);
    }

    public function isCyclic($current, string $key, \RecursiveArrayIterator $iterator): bool {
        var_dump($current);
        if (!is_array($current)) {
            return false;
        }
        // Avoid infinite loops by checking if we've been here before.
        // e.g. View > query > view > query ...
        if (isset($current['#override_mode_breadcrumb'])) {
            return false;
        }
        return true;
    }
}

$test_array['e']['p'][] = ['a', 'a'];
$test_array['e']['p'][] = ['b', 'b'];
$test_array['e']['p'][] = ['c', 'c'];
$serialized = serialize($test_array);
$unserialized = unserialize($serialized);

$test_class = new RecursiveFilterTest();
$test_class->traverse($unserialized);

echo "Done\n";

?>
--EXPECT--
array(1) {
  ["p"]=>
  array(3) {
    [0]=>
    array(2) {
      [0]=>
      string(1) "a"
      [1]=>
      string(1) "a"
    }
    [1]=>
    array(2) {
      [0]=>
      string(1) "b"
      [1]=>
      string(1) "b"
    }
    [2]=>
    array(2) {
      [0]=>
      string(1) "c"
      [1]=>
      string(1) "c"
    }
  }
}
array(3) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "a"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "b"
    [1]=>
    string(1) "b"
  }
  [2]=>
  array(2) {
    [0]=>
    string(1) "c"
    [1]=>
    string(1) "c"
  }
}
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "a"
}
string(1) "a"
string(1) "a"
array(2) {
  [0]=>
  string(1) "b"
  [1]=>
  string(1) "b"
}
string(1) "b"
string(1) "b"
array(2) {
  [0]=>
  string(1) "c"
  [1]=>
  string(1) "c"
}
string(1) "c"
string(1) "c"
array(1) {
  ["p"]=>
  array(3) {
    [0]=>
    array(2) {
      [0]=>
      string(1) "a"
      [1]=>
      string(1) "a"
    }
    [1]=>
    array(2) {
      [0]=>
      string(1) "b"
      [1]=>
      string(1) "b"
    }
    [2]=>
    array(2) {
      [0]=>
      string(1) "c"
      [1]=>
      string(1) "c"
    }
  }
}
array(3) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "a"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "b"
    [1]=>
    string(1) "b"
  }
  [2]=>
  array(2) {
    [0]=>
    string(1) "c"
    [1]=>
    string(1) "c"
  }
}
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "a"
}
string(1) "a"
string(1) "a"
array(2) {
  [0]=>
  string(1) "b"
  [1]=>
  string(1) "b"
}
string(1) "b"
string(1) "b"
array(2) {
  [0]=>
  string(1) "c"
  [1]=>
  string(1) "c"
}
string(1) "c"
string(1) "c"
Done
