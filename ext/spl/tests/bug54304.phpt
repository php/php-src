--TEST--
Bug #54304 (Setting replacement value for RegexIterator doesn't work)
--FILE--
<?php
class foo extends ArrayIterator {
	public function __construct( ) {
		parent::__construct(array(
			'test3'=>'test999'));
	}
}

$h = new foo;
$i = new RegexIterator($h, '/^test(.*)/', RegexIterator::REPLACE);
$i->replacement = 42;
var_dump($i->replacement);
foreach ($i as $name=>$value) {
	var_dump($name, $value);
}
var_dump($i->replacement);
?>
--EXPECT--
int(42)
string(5) "test3"
string(2) "42"
int(42)

