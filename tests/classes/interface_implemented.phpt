--TEST--
ZE2 An interface is inherited
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

interface if_a {
	function f_a();
}
	
interface if_b extends if_a {
	function f_b();
}

class base {
	function _is_a($sub) {
		echo 'is_a('.get_class($this).', '.$sub.') = '.(($this instanceof $sub) ? 'yes' : 'no')."\n";
	}
	function test() {
		echo $this->_is_a('base');
		echo $this->_is_a('derived_a');
		echo $this->_is_a('derived_b');
		echo $this->_is_a('derived_c');
		echo $this->_is_a('derived_d');
		echo $this->_is_a('if_a');
		echo $this->_is_a('if_b');
		echo "\n";
	}
}

class derived_a extends base implements if_a {
	function f_a() {}
}

class derived_b extends base implements if_a, if_b {
	function f_a() {}
	function f_b() {}
}

class derived_c extends derived_a implements if_b {
	function f_b() {}
}

class derived_d extends derived_c {
}

$t = new base();
$t->test();

$t = new derived_a();
$t->test();

$t = new derived_b();
$t->test();

$t = new derived_c();
$t->test();

$t = new derived_d();
$t->test();

?>
--EXPECTF--
is_a(base, base) = yes
is_a(base, derived_a) = no
is_a(base, derived_b) = no
is_a(base, derived_c) = no
is_a(base, derived_d) = no
is_a(base, if_a) = no
is_a(base, if_b) = no

is_a(derived_a, base) = yes
is_a(derived_a, derived_a) = yes
is_a(derived_a, derived_b) = no
is_a(derived_a, derived_c) = no
is_a(derived_a, derived_d) = no
is_a(derived_a, if_a) = yes
is_a(derived_a, if_b) = no

is_a(derived_b, base) = yes
is_a(derived_b, derived_a) = no
is_a(derived_b, derived_b) = yes
is_a(derived_b, derived_c) = no
is_a(derived_b, derived_d) = no
is_a(derived_b, if_a) = yes
is_a(derived_b, if_b) = yes

is_a(derived_c, base) = yes
is_a(derived_c, derived_a) = yes
is_a(derived_c, derived_b) = no
is_a(derived_c, derived_c) = yes
is_a(derived_c, derived_d) = no
is_a(derived_c, if_a) = yes
is_a(derived_c, if_b) = yes

is_a(derived_d, base) = yes
is_a(derived_d, derived_a) = yes
is_a(derived_d, derived_b) = no
is_a(derived_d, derived_c) = yes
is_a(derived_d, derived_d) = yes
is_a(derived_d, if_a) = yes
is_a(derived_d, if_b) = yes
