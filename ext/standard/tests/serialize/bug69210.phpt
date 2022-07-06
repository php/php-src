--TEST--
serialize() integrity with non string on __sleep
--FILE--
<?php
class testString
{
    public $a = true;

    public function __sleep()
    {
        return array('a', '1');
    }
}

class testInteger
{
    public $a = true;

    public function __sleep()
    {
        return array('a', 1);
    }
}

$cs = new testString();
$ci = new testInteger();

$ss =  @serialize($cs);
echo $ss . "\n";

$si = @serialize($ci);
echo $si . "\n";

var_dump(unserialize($ss));
var_dump(unserialize($si));
?>
--EXPECT--
O:10:"testString":1:{s:1:"a";b:1;}
O:11:"testInteger":1:{s:1:"a";b:1;}
object(testString)#3 (1) {
  ["a"]=>
  bool(true)
}
object(testInteger)#3 (1) {
  ["a"]=>
  bool(true)
}
