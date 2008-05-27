--TEST--
Bug #21957 (serialize() mangles objects with __sleep)
--FILE--
<?php 
class test
{
	public $a, $b;

	function test()
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
--EXPECT--
array(2) {
  [u"one"]=>
  unicode(3) "ABC"
  [u"two"]=>
  object(test)#1 (2) {
    [u"a"]=>
    int(7)
    [u"b"]=>
    int(2)
  }
}
a:2:{U:3:"one";U:3:"ABC";U:3:"two";N;}
array(2) {
  [u"one"]=>
  unicode(3) "ABC"
  [u"two"]=>
  NULL
}
