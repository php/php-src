--TEST--
Bug #50579 (RegexIterator::REPLACE doesn't work)
--FILE--
<?php

class foo extends ArrayIterator {
	public function __construct( ) {
		parent::__construct(array(
		'test1'=>'test888', 
		'test2'=>'what?', 
		'test3'=>'test999'));
	}
}
$h = new foo;
$i = new RegexIterator($h, '/^test(.*)/', RegexIterator::REPLACE);
$i->replacement = '[$0]';
foreach ($i as $name=>$value) {
	echo $name . '=>' . $value . "\n";
}
  
$i->replacement = '$1';
foreach ($i as $name=>$value) {
	echo $name . '=>' . $value . "\n";
}

$h = new foo;
$i = new RegexIterator($h, '/^test(.*)/', RegexIterator::REPLACE);
$i->replacement = '[$1]';
foreach ($i as $name=>$value) {
	echo $name . '=>' . $value . "\n";
}

?>
--EXPECTF--
test1=>[test888]
test3=>[test999]
test1=>888
test3=>999
test1=>[888]
test3=>[999]
