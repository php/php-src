--TEST--
ZE2 object cloning, 3
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php
class base {
	private $p1 = 1;
	protected $p2 = 2;
	public $p3;
	public function __clone() {
	}
};

class test {
	public $p1 = 4;
	protected $p4 = 5;
	public $p5;
	public function __clone() {
	}
}

$obj = new test;
$obj->p2 = 'A';
$obj->p3 = 'B';
$copy = $obj->__clone();
$copy->p3 = 'C';
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
    [p1] => 4
    [p4:protected] => 5
    [p5] => 
    [p2] => A
    [p3] => B
)
Clown
test Object
(
    [p1] => 4
    [p4:protected] => 5
    [p5] => 
    [p1:private] => 1
    [p2] => A
    [p3] => C
)
Done
