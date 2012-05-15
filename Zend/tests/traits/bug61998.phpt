--TEST--
Bug #61998 (Crash when declare trait after class if user defined the same method as aliased one)
--FILE--
<?php

class Class1 {
	use T {
		func as newFunc;
	}

	public function func() {
		echo "From Class1::func\n";
	}
}

class Class2 {
	use T {
		func as newFunc2;
	}

	public function func() {
		echo "From Class2::func\n";
	}
}

class Class3 {
	use T;
}

trait T {
	public function func() {
		echo "From trait T\n";
	}
}

$class1 = new Class1();
$class2 = new Class2();
$class3 = new Class3();

$class1->func();
$class2->func();
$class3->func();
?>
==DONE==
--EXPECT--
From Class1::func
From Class2::func
From trait T
==DONE==
