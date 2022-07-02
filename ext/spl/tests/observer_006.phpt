--TEST--
SPL: SplObjectStorage with accociatied information
--FILE--
<?php

class TestClass
{
    public $test = 25;

    public function __construct($test = 42)
    {
        $this->test = $test;
    }
}

class MyStorage extends SplObjectStorage
{
    public $bla = 25;

    public function __construct($bla = 26)
    {
        $this->bla = $bla;
    }
}

$storage = new MyStorage();

foreach(array(1=>"foo",2=>42) as $key => $value)
{
     $storage->attach(new TestClass($key), $value);
}

var_dump(count($storage));

foreach($storage as $object)
{
    var_dump($object->test);
}

var_dump($storage);

var_dump(serialize($storage));
echo "===UNSERIALIZE===\n";

$storage2 = unserialize(serialize($storage));

var_dump(count($storage2));

foreach($storage2 as $object)
{
    var_dump($object->test);
}

var_dump($storage2);
$storage->attach(new TestClass(3), new stdClass);
$storage->attach(new TestClass(4), new TestClass(5));
echo "===UNSERIALIZE2===\n";
var_dump(unserialize(serialize($storage)));
$storage->rewind();
$storage->next();
var_dump($storage->key());
var_dump($storage->current());
var_dump($storage->getInfo());
$storage->setInfo("bar");
var_dump($storage->getInfo());
echo "===UNSERIALIZE3===\n";
var_dump(unserialize(serialize($storage)));
$storage->rewind();
$storage->next();
$storage->next();
var_dump($storage->key());
var_dump($storage->current());
$storage->attach($storage->current(), "replaced");
echo "===UNSERIALIZE4===\n";
var_dump(unserialize(serialize($storage)));

?>
--EXPECTF--
int(2)
int(1)
int(2)
object(MyStorage)#%d (2) {
  ["bla"]=>
  int(26)
  ["storage":"SplObjectStorage":private]=>
  array(2) {
    [0]=>
    array(2) {
      ["obj"]=>
      object(TestClass)#%d (1) {
        ["test"]=>
        int(1)
      }
      ["inf"]=>
      string(3) "foo"
    }
    [1]=>
    array(2) {
      ["obj"]=>
      object(TestClass)#%d (1) {
        ["test"]=>
        int(2)
      }
      ["inf"]=>
      int(42)
    }
  }
}
string(%d) "%s"
===UNSERIALIZE===
int(2)
int(1)
int(2)
object(MyStorage)#%d (2) {
  ["bla"]=>
  int(26)
  ["storage":"SplObjectStorage":private]=>
  array(2) {
    [0]=>
    array(2) {
      ["obj"]=>
      object(TestClass)#%d (1) {
        ["test"]=>
        int(1)
      }
      ["inf"]=>
      string(3) "foo"
    }
    [1]=>
    array(2) {
      ["obj"]=>
      object(TestClass)#%d (1) {
        ["test"]=>
        int(2)
      }
      ["inf"]=>
      int(42)
    }
  }
}
===UNSERIALIZE2===
object(MyStorage)#%d (2) {
  ["bla"]=>
  int(26)
  ["storage":"SplObjectStorage":private]=>
  array(4) {
    [0]=>
    array(2) {
      ["obj"]=>
      object(TestClass)#%d (1) {
        ["test"]=>
        int(1)
      }
      ["inf"]=>
      string(3) "foo"
    }
    [1]=>
    array(2) {
      ["obj"]=>
      object(TestClass)#%d (1) {
        ["test"]=>
        int(2)
      }
      ["inf"]=>
      int(42)
    }
    [2]=>
    array(2) {
      ["obj"]=>
      object(TestClass)#%d (1) {
        ["test"]=>
        int(3)
      }
      ["inf"]=>
      object(stdClass)#%d (0) {
      }
    }
    [3]=>
    array(2) {
      ["obj"]=>
      object(TestClass)#%d (1) {
        ["test"]=>
        int(4)
      }
      ["inf"]=>
      object(TestClass)#%d (1) {
        ["test"]=>
        int(5)
      }
    }
  }
}
int(1)
object(TestClass)#%d (1) {
  ["test"]=>
  int(2)
}
int(42)
string(3) "bar"
===UNSERIALIZE3===
object(MyStorage)#%d (2) {
  ["bla"]=>
  int(26)
  ["storage":"SplObjectStorage":private]=>
  array(4) {
    [0]=>
    array(2) {
      ["obj"]=>
      object(TestClass)#%d (1) {
        ["test"]=>
        int(1)
      }
      ["inf"]=>
      string(3) "foo"
    }
    [1]=>
    array(2) {
      ["obj"]=>
      object(TestClass)#%d (1) {
        ["test"]=>
        int(2)
      }
      ["inf"]=>
      string(3) "bar"
    }
    [2]=>
    array(2) {
      ["obj"]=>
      object(TestClass)#%d (1) {
        ["test"]=>
        int(3)
      }
      ["inf"]=>
      object(stdClass)#%d (0) {
      }
    }
    [3]=>
    array(2) {
      ["obj"]=>
      object(TestClass)#%d (1) {
        ["test"]=>
        int(4)
      }
      ["inf"]=>
      object(TestClass)#%d (1) {
        ["test"]=>
        int(5)
      }
    }
  }
}
int(2)
object(TestClass)#7 (1) {
  ["test"]=>
  int(3)
}
===UNSERIALIZE4===
object(MyStorage)#%d (2) {
  ["bla"]=>
  int(26)
  ["storage":"SplObjectStorage":private]=>
  array(4) {
    [0]=>
    array(2) {
      ["obj"]=>
      object(TestClass)#%d (1) {
        ["test"]=>
        int(1)
      }
      ["inf"]=>
      string(3) "foo"
    }
    [1]=>
    array(2) {
      ["obj"]=>
      object(TestClass)#%d (1) {
        ["test"]=>
        int(2)
      }
      ["inf"]=>
      string(3) "bar"
    }
    [2]=>
    array(2) {
      ["obj"]=>
      object(TestClass)#%d (1) {
        ["test"]=>
        int(3)
      }
      ["inf"]=>
      string(8) "replaced"
    }
    [3]=>
    array(2) {
      ["obj"]=>
      object(TestClass)#%d (1) {
        ["test"]=>
        int(4)
      }
      ["inf"]=>
      object(TestClass)#%d (1) {
        ["test"]=>
        int(5)
      }
    }
  }
}
