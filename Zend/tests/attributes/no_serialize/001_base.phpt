--TEST--
#[\NoSerialize]: Basic test.
--FILE--
<?php

class Base {
    #[\NoSerialize]
    public $b1 = 'a';
    protected $b2 = 'b';
    private $b3 = 'c';
}

class Child extends Base {
    public $b1 = 'b';

    function __construct (
        #[\NoSerialize]
        public $c = 'c',
        public $d = 'd',
    ) {}
}

class Unserializable {
    #[\NoSerialize]
    public $a;
    public $b;
}

$base = new Base;
echo serialize($base), PHP_EOL;
echo serialize(new Child), PHP_EOL;
$base->b1 = 'b';
var_dump(unserialize(serialize($base)));
var_dump(unserialize('O:14:"Unserializable":2:{s:1:"a";s:1:"a";s:1:"b";s:1:"b";}'));

?>
--EXPECTF--
O:4:"Base":2:{s:5:"%0*%0b2";s:1:"b";s:8:"%0Base%0b3";s:1:"c";}
O:5:"Child":4:{s:2:"b1";s:1:"b";s:5:"%0*%0b2";s:1:"b";s:8:"%0Base%0b3";s:1:"c";s:1:"d";s:1:"d";}
object(Base)#%d (3) {
  ["b1"]=>
  string(1) "a"
  ["b2":protected]=>
  string(1) "b"
  ["b3":"Base":private]=>
  string(1) "c"
}
object(Unserializable)#2 (2) {
  ["a"]=>
  string(1) "a"
  ["b"]=>
  string(1) "b"
}
