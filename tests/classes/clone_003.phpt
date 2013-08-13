--TEST--
ZE2 object cloning, 3
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php
class base {
	protected $p1 = 'base:1';
	public $p2 = 'base:2';
	public $p3 = 'base:3';
	public $p4 = 'base:4';
	public $p5 = 'base:5';
	private $p6 = 'base:6';
	public function __clone() {
	}
};

class test extends base {
	public $p1 = 'test:1';
	public $p3 = 'test:3';
	public $p4 = 'test:4';
	public $p5 = 'test:5';
	public function __clone() {
		$this->p5 = 'clone:5';
	}
}

$obj = new test;
$obj->p4 = 'A';
$copy = clone $obj;
echo "Object\n";
print_r($obj);
echo "Clown\n";
print_r($copy);
echo "Done\n";
?>
--EXPECT--
Object
test Object
(
    [p1] => test:1
    [p3] => test:3
    [p4] => A
    [p5] => test:5
    [p2] => base:2
    [p6:base:private] => base:6
)
Clown
test Object
(
    [p1] => test:1
    [p3] => test:3
    [p4] => A
    [p5] => clone:5
    [p2] => base:2
    [p6:base:private] => base:6
)
Done
