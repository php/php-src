--TEST--
ZE2 A private method can be overwritten in a second derived class
--FILE--
<?php

class first {
	private function show() {
		echo "Call show()\n";
	}

	public function do_show() {
		$this->show();
	}
}

$t1 = new first();
$t1->do_show();

class second extends first {
}

//$t2 = new second();
//$t2->do_show();

class third extends second {
	private function show() {
		echo "Call show()\n";
	}
}

$t3 = new third();
$t3->do_show();

echo "Done\n";
?>
--EXPECT--
Call show()
Call show()
Done
