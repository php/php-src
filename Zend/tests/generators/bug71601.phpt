--TEST--
Bug #71601 (finally block not executed after yield from)
--FILE--
<?php

function gen1() {
	try {
		yield 1;
		yield 2;
		return true;
	} finally {
		echo "Inner finally\n";
	}
}

function gen2() {
	try {
		echo "Entered try/catch\n";
		var_dump(yield from gen1());
	} finally {
		echo "Finally\n";
	}
}

$generator = gen2();

var_dump($generator->current());

unset($generator);

echo "Done\n";

?>
--EXPECT--
Entered try/catch
int(1)
Inner finally
Finally
Done

