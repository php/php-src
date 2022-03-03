--TEST--
Bug #79862: Public non-static property in child should take priority over private static
--FILE--
<?php

#[AllowDynamicProperties]
class a {
    private static $prop1;
    private static $prop2;
    private $prop3;
    private $prop4;
    private static $prop5;
    private static $prop6;
    public function __construct() {
        $this->prop1 = 1;
        $this->prop2 = 2;
        $this->prop3 = 3;
        $this->prop4 = 4;
        $this->prop5 = 5;
        $this->prop6 = 6;
        var_dump(self::$prop1);
        var_dump(self::$prop2);
        var_dump(self::$prop5);
        var_dump(self::$prop6);
        var_dump($this);
    }
}
class c extends a {
    public $prop1;
    protected $prop2;
    public static $prop3;
    protected static $prop4;
    public static $prop5;
    protected static $prop6;
}

$c = new c;

?>
--EXPECTF--
Notice: Accessing static property c::$prop5 as non static in %s on line %d

Notice: Accessing static property c::$prop6 as non static in %s on line %d
NULL
NULL
NULL
NULL
object(c)#1 (6) {
  ["prop3":"a":private]=>
  int(3)
  ["prop4":"a":private]=>
  int(4)
  ["prop1"]=>
  int(1)
  ["prop2":protected]=>
  int(2)
  ["prop5"]=>
  int(5)
  ["prop6"]=>
  int(6)
}
