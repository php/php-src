--TEST--
Bug #48770 (call_user_func_array() fails to call parent from inheriting class)
--XFAIL--
See Bug #48770
--FILE--
<?php

class A {
	public function func($str) {
		var_dump(__METHOD__ .': '. $str);
	}
	private function func2($str) {
		var_dump(__METHOD__ .': '. $str);
	}
	protected function func3($str) {
		var_dump(__METHOD__ .': '. $str);
	}
	private function func22($str) {
		var_dump(__METHOD__ .': '. $str);
	}
}

class B extends A {
	public function func($str) {
		static $avoid_crash = 0;

		if ($avoid_crash++ == 1) {
			print "This method shouldn't be called when using parent::func!\n";
			return;
		}

		call_user_func_array(array($this, 'parent::func'), array($str));
	}
	private function func2($str) {
		var_dump(__METHOD__ .': '. $str);
	}
	protected function func3($str) {
		var_dump(__METHOD__ .': '. $str);
	}
}

class C extends B {
	public function func($str) {
		parent::func($str);
	}
}

$c = new C;
$c->func('This should work!');

?>
--EXPECT--
string(26) "A::func: This should work!"
