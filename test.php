<?php
if (isset($include)) {
	include (sprintf("%s/web-bootstrap.php", dirname(__FILE__)));
}

$stdout = fopen("php://stdout", "w+");

class phpdbg {
	private $sprintf = "%s: %s\n";

	public function isGreat($greeting = null) {
		printf($this->sprintf, __METHOD__, $greeting);
		return $this;
	}
}

function mine() {
	var_dump(func_get_args());
}

function test($x, $y = 0) {
	$var = $x + 1;
	$var += 2;
	$var <<= 3;

	$foo = function () {
		echo "bar!\n";
	};

	$foo();

	yield $var;
}

$dbg = new phpdbg();

var_dump(
    $dbg->isGreat("PHP Rocks!!"));

foreach (test(1,2) as $gen)
	continue;

echo "it works!\n";

if (isset($dump))
	var_dump($_SERVER);

function phpdbg_test_ob()
{
	echo 'Start';
	ob_start();
	echo 'Hello';
	$b = ob_get_clean();
	echo 'End';
	echo $b;
}

$array = [
	1,
	2,
	[3, 4],
	[5, 6],
];

$array[] = 7;

array_walk($array, function (&$item) {
	if (is_array($item))
		$item[0] += 2;
	else
		$item -= 1;
});

class testClass {
	public $a = 2;
	protected  $b = [1, 3];
	private $c = 7;
}

$obj = new testClass;

$test = $obj->a;

$obj->a += 2;
$test -= 2;

unset($obj);
