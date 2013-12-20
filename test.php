<?php
if (isset($include)) {
	include (sprintf("%s/web-bootstrap.php", dirname(__FILE__)));
}

$stdout = fopen("php://stdout", "w+");

class phpdbg {
    public function isGreat($greeting = null) {
        printf(
            "%s: %s\n", __METHOD__, $greeting);
        return $this;
    }
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
