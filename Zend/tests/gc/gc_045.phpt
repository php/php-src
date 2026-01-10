--TEST--
GC 045: Total count persisted when GC is rerun due to destructor call
--INI--
zend.enable_gc=1
--FILE--
<?php
class GlobalData
{
    public static Bar $bar;
}

class Value
{
    public function __destruct()
    {
        new Bar();
    }
}

class Bar
{
    public function __construct()
    {
        GlobalData::$bar = $this;
    }
}

class Foo
{
    public Foo $selfRef;
    public Value $val;

    public function __construct(Value $val)
    {
        $this->val = $val;
        $this->selfRef = $this;
    }
}

for ($j = 0; $j < 10; $j++) {
    for ($i = 0; $i < 3000; $i++) {
        new Foo(new Value());
    }
}

var_dump(gc_status());
?>
--EXPECTF--
array(12) {
  ["running"]=>
  bool(false)
  ["protected"]=>
  bool(false)
  ["full"]=>
  bool(false)
  ["runs"]=>
  int(10)
  ["collected"]=>
  int(25000)
  ["threshold"]=>
  int(10001)
  ["buffer_size"]=>
  int(16384)
  ["roots"]=>
  int(10000)
  ["application_time"]=>
  float(%f)
  ["collector_time"]=>
  float(%f)
  ["destructor_time"]=>
  float(%f)
  ["free_time"]=>
  float(%f)
}
