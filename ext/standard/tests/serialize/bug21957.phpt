--TEST--
Bug #21957 (serialize() mangles objects with __sleep)
--FILE--
<?php
class test
{
    public $a, $b;

    function __construct()
    {
        $this->a = 7;
        $this->b = 2;
    }

    function __sleep()
    {
        $this->b = 0;
    }
}

$t['one'] = 'ABC';
$t['two'] = new test();

var_dump($t);

$s =  @serialize($t);
echo $s . "\n";

var_dump(unserialize($s));
?>
--EXPECTF--
Deprecated: The __sleep() serialization magic method has been deprecated. Implement __serialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d
array(2) {
  ["one"]=>
  string(3) "ABC"
  ["two"]=>
  object(test)#1 (2) {
    ["a"]=>
    int(7)
    ["b"]=>
    int(2)
  }
}
a:2:{s:3:"one";s:3:"ABC";s:3:"two";N;}
array(2) {
  ["one"]=>
  string(3) "ABC"
  ["two"]=>
  NULL
}
