<?php

if (isset($include))
	include (sprintf("%s/web-bootstrap.php", dirname(__FILE__)));

class phpdbg {
    public function isGreat($greeting = null) {
        printf(
            "%s: %s\n", __METHOD__, $greeting);
        return $this;
    }
}

function test() {
	$var = 1 + 1;
	$var += 2;
	$var <<= 3;

	$foo = function () {};

	$foo();

	return $var;
}

$dbg = new phpdbg();

$test = 1;

var_dump(
    $dbg->isGreat("PHP Rocks !!"));

test();

echo "it works!\n";

if (isset($dump))
	var_dump($_SERVER);
?>
