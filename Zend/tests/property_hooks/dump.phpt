--TEST--
Dumping object with property hooks
--FILE--
<?php

class Test {
    public $addedHooks = 'addedHooks';
    public $virtual {
        get { return strtoupper('virtual'); }
    }
    public $backed = 'backed' {
        get { return strtoupper($this->backed); }
        set { $this->backed = $value; }
    }
    public $writeOnly {
        set {}
    }
    private $private = 'private' {
        get { return strtoupper($this->private); }
        set { $this->private = $value; }
    }
    private $changed = 'changed Test' {
        get { return strtoupper($this->changed); }
    }
    public function dumpTest() {
        var_dump($this);
        var_dump(get_object_vars($this));
        var_dump(get_mangled_object_vars($this));
        var_export($this);
        echo "\n";
        echo json_encode($this), "\n";
        var_dump((array) $this);
    }
}

class Child extends Test {
    public $addedHooks {
        get { return strtoupper(parent::$addedHooks::get()); }
    }
    private $changed = 'changed Child' {
        get { return strtoupper($this->changed); }
    }
    public function dumpChild() {
        var_dump($this);
        var_dump(get_object_vars($this));
        var_export($this);
        echo "\n";
        echo json_encode($this), "\n";
        var_dump((array) $this);
    }
}

function dump($test) {
    var_dump($test);
    var_dump(get_object_vars($test));
    var_export($test);
    echo "\n";
    echo json_encode($test), "\n";
    var_dump((array) $test);
}

echo "dump(Test):\n";
dump(new Test);

echo "\n\ndump(Child):\n";
dump(new Child);

echo "\n\nChild::dumpTest():\n";
(new Child)->dumpTest();

echo "\n\nChild::dumpChild():\n";
(new Child)->dumpChild();

?>
--EXPECTF--
dump(Test):
object(Test)#%d (4) {
  ["addedHooks"]=>
  string(10) "addedHooks"
  ["backed"]=>
  string(6) "backed"
  ["private":"Test":private]=>
  string(7) "private"
  ["changed":"Test":private]=>
  string(12) "changed Test"
}
array(3) {
  ["addedHooks"]=>
  string(10) "addedHooks"
  ["virtual"]=>
  string(7) "VIRTUAL"
  ["backed"]=>
  string(6) "BACKED"
}
\Test::__set_state(array(
   'addedHooks' => 'addedHooks',
   'virtual' => 'VIRTUAL',
   'backed' => 'BACKED',
   'private' => 'PRIVATE',
   'changed' => 'CHANGED TEST',
))
{"addedHooks":"addedHooks","virtual":"VIRTUAL","backed":"BACKED"}
array(4) {
  ["addedHooks"]=>
  string(10) "addedHooks"
  ["backed"]=>
  string(6) "backed"
  ["%0Test%0private"]=>
  string(7) "private"
  ["%0Test%0changed"]=>
  string(12) "changed Test"
}


dump(Child):
object(Child)#%d (5) {
  ["addedHooks"]=>
  string(10) "addedHooks"
  ["backed"]=>
  string(6) "backed"
  ["private":"Test":private]=>
  string(7) "private"
  ["changed":"Test":private]=>
  string(12) "changed Test"
  ["changed":"Child":private]=>
  string(13) "changed Child"
}
array(3) {
  ["addedHooks"]=>
  string(10) "ADDEDHOOKS"
  ["virtual"]=>
  string(7) "VIRTUAL"
  ["backed"]=>
  string(6) "BACKED"
}
\Child::__set_state(array(
   'addedHooks' => 'ADDEDHOOKS',
   'virtual' => 'VIRTUAL',
   'backed' => 'BACKED',
   'private' => 'PRIVATE',
   'changed' => 'CHANGED TEST',
   'changed' => 'CHANGED CHILD',
))
{"addedHooks":"ADDEDHOOKS","virtual":"VIRTUAL","backed":"BACKED"}
array(5) {
  ["addedHooks"]=>
  string(10) "addedHooks"
  ["backed"]=>
  string(6) "backed"
  ["%0Test%0private"]=>
  string(7) "private"
  ["%0Test%0changed"]=>
  string(12) "changed Test"
  ["%0Child%0changed"]=>
  string(13) "changed Child"
}


Child::dumpTest():
object(Child)#%d (5) {
  ["addedHooks"]=>
  string(10) "addedHooks"
  ["backed"]=>
  string(6) "backed"
  ["private":"Test":private]=>
  string(7) "private"
  ["changed":"Test":private]=>
  string(12) "changed Test"
  ["changed":"Child":private]=>
  string(13) "changed Child"
}
array(5) {
  ["addedHooks"]=>
  string(10) "ADDEDHOOKS"
  ["virtual"]=>
  string(7) "VIRTUAL"
  ["backed"]=>
  string(6) "BACKED"
  ["private"]=>
  string(7) "PRIVATE"
  ["changed"]=>
  string(12) "CHANGED TEST"
}
array(5) {
  ["addedHooks"]=>
  string(10) "addedHooks"
  ["backed"]=>
  string(6) "backed"
  ["%0Test%0private"]=>
  string(7) "private"
  ["%0Test%0changed"]=>
  string(12) "changed Test"
  ["%0Child%0changed"]=>
  string(13) "changed Child"
}
\Child::__set_state(array(
   'addedHooks' => 'ADDEDHOOKS',
   'virtual' => 'VIRTUAL',
   'backed' => 'BACKED',
   'private' => 'PRIVATE',
   'changed' => 'CHANGED TEST',
   'changed' => 'CHANGED CHILD',
))
{"addedHooks":"ADDEDHOOKS","virtual":"VIRTUAL","backed":"BACKED"}
array(5) {
  ["addedHooks"]=>
  string(10) "addedHooks"
  ["backed"]=>
  string(6) "backed"
  ["%0Test%0private"]=>
  string(7) "private"
  ["%0Test%0changed"]=>
  string(12) "changed Test"
  ["%0Child%0changed"]=>
  string(13) "changed Child"
}


Child::dumpChild():
object(Child)#%d (5) {
  ["addedHooks"]=>
  string(10) "addedHooks"
  ["backed"]=>
  string(6) "backed"
  ["private":"Test":private]=>
  string(7) "private"
  ["changed":"Test":private]=>
  string(12) "changed Test"
  ["changed":"Child":private]=>
  string(13) "changed Child"
}
array(4) {
  ["addedHooks"]=>
  string(10) "ADDEDHOOKS"
  ["virtual"]=>
  string(7) "VIRTUAL"
  ["backed"]=>
  string(6) "BACKED"
  ["changed"]=>
  string(13) "CHANGED CHILD"
}
\Child::__set_state(array(
   'addedHooks' => 'ADDEDHOOKS',
   'virtual' => 'VIRTUAL',
   'backed' => 'BACKED',
   'private' => 'PRIVATE',
   'changed' => 'CHANGED TEST',
   'changed' => 'CHANGED CHILD',
))
{"addedHooks":"ADDEDHOOKS","virtual":"VIRTUAL","backed":"BACKED"}
array(5) {
  ["addedHooks"]=>
  string(10) "addedHooks"
  ["backed"]=>
  string(6) "backed"
  ["%0Test%0private"]=>
  string(7) "private"
  ["%0Test%0changed"]=>
  string(12) "changed Test"
  ["%0Child%0changed"]=>
  string(13) "changed Child"
}
