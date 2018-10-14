--TEST--
Testing call_user_func inside namespace
--FILE--
<?php

namespace testing {
	function foobar($str) {
		var_dump($str);
	}

	abstract class bar {
		protected function prot($str) {
			print "Shouldn't be called!\n";
		}
	}
	class foo extends bar {
		private function priv($str) {
			print "Shouldn't be called!\n";
		}
	}

	call_user_func(__NAMESPACE__ .'\foobar', 'foobar');

	$class =  __NAMESPACE__ .'\foo';
	call_user_func(array(new $class, 'priv'), 'foobar');
	call_user_func(array(new $class, 'prot'), 'foobar');
}

?>
--EXPECTF--
%string|unicode%(6) "foobar"

Warning: call_user_func() expects parameter 1 to be a valid callback, cannot access private method testing\foo::priv() in %s on line %d

Warning: call_user_func() expects parameter 1 to be a valid callback, cannot access protected method testing\foo::prot() in %s on line %d
