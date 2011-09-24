--TEST--
is_a and is_subclass_behaviour
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
		
		echo "\n>>> With Defined class\n";
		echo str_pad('is_a( OBJECT:'.get_class($this).', '.$sub.') = ', 60) . (is_a($this, $sub) ? 'yes' : 'no')."\n";
		echo str_pad('is_a( STRING:'.get_class($this).', '.$sub.') = ', 60). (is_a(get_class($this), $sub) ? 'yes' : 'no')."\n";
		echo str_pad('is_subclass_of( OBJECT:'.get_class($this).', '.$sub.') = ', 60).  (is_subclass_of($this, $sub) ? 'yes' : 'no')."\n";
		echo str_pad('is_subclass_of( STRING:'.get_class($this).', '.$sub.') = ', 60). (is_subclass_of(get_class($this), $sub) ? 'yes' : 'no')."\n";
		
		// with autoload options..
		echo ">>> With Undefined\n";
		echo  str_pad('is_a( STRING:undefB, '.$sub.',true) = ', 60). (is_a('undefB', $sub, true) ? 'yes' : 'no')."\n";
		echo  str_pad('is_a( STRING:undefB, '.$sub.') = ', 60). (is_a('undefB', $sub) ? 'yes' : 'no')."\n";
		echo  str_pad('is_subclass_of( STRING:undefB, '.$sub.',false) = ', 60). (is_subclass_of('undefB', $sub, false) ? 'yes' : 'no')."\n";
		echo  str_pad('is_subclass_of( STRING:undefB, '.$sub.') = ', 60). (is_subclass_of('undefB', $sub) ? 'yes' : 'no')."\n";
	}
	function test() {
		echo $this->_is_a('base');
		echo $this->_is_a('derived_a');
		echo $this->_is_a('derived_b');
		echo $this->_is_a('derived_c');
		echo $this->_is_a('derived_d');
		echo $this->_is_a('if_a');
		echo $this->_is_a('if_b');
		echo $this->_is_a('undefA');
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
eval('
  function __autoload($name)
  {
      echo ">>>> In __autoload: ";
      var_dump($name);
  }
');

echo "NOW WITH AUTOLOAD\n\n";

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
>>> With Defined class
is_a( OBJECT:base, base) =                                  yes
is_a( STRING:base, base) =                                  no
is_subclass_of( OBJECT:base, base) =                        no
is_subclass_of( STRING:base, base) =                        no
>>> With Undefined
is_a( STRING:undefB, base,true) =                           no
is_a( STRING:undefB, base) =                                no
is_subclass_of( STRING:undefB, base,false) =                no
is_subclass_of( STRING:undefB, base) =                      no

>>> With Defined class
is_a( OBJECT:base, derived_a) =                             no
is_a( STRING:base, derived_a) =                             no
is_subclass_of( OBJECT:base, derived_a) =                   no
is_subclass_of( STRING:base, derived_a) =                   no
>>> With Undefined
is_a( STRING:undefB, derived_a,true) =                      no
is_a( STRING:undefB, derived_a) =                           no
is_subclass_of( STRING:undefB, derived_a,false) =           no
is_subclass_of( STRING:undefB, derived_a) =                 no

>>> With Defined class
is_a( OBJECT:base, derived_b) =                             no
is_a( STRING:base, derived_b) =                             no
is_subclass_of( OBJECT:base, derived_b) =                   no
is_subclass_of( STRING:base, derived_b) =                   no
>>> With Undefined
is_a( STRING:undefB, derived_b,true) =                      no
is_a( STRING:undefB, derived_b) =                           no
is_subclass_of( STRING:undefB, derived_b,false) =           no
is_subclass_of( STRING:undefB, derived_b) =                 no

>>> With Defined class
is_a( OBJECT:base, derived_c) =                             no
is_a( STRING:base, derived_c) =                             no
is_subclass_of( OBJECT:base, derived_c) =                   no
is_subclass_of( STRING:base, derived_c) =                   no
>>> With Undefined
is_a( STRING:undefB, derived_c,true) =                      no
is_a( STRING:undefB, derived_c) =                           no
is_subclass_of( STRING:undefB, derived_c,false) =           no
is_subclass_of( STRING:undefB, derived_c) =                 no

>>> With Defined class
is_a( OBJECT:base, derived_d) =                             no
is_a( STRING:base, derived_d) =                             no
is_subclass_of( OBJECT:base, derived_d) =                   no
is_subclass_of( STRING:base, derived_d) =                   no
>>> With Undefined
is_a( STRING:undefB, derived_d,true) =                      no
is_a( STRING:undefB, derived_d) =                           no
is_subclass_of( STRING:undefB, derived_d,false) =           no
is_subclass_of( STRING:undefB, derived_d) =                 no

>>> With Defined class
is_a( OBJECT:base, if_a) =                                  no
is_a( STRING:base, if_a) =                                  no
is_subclass_of( OBJECT:base, if_a) =                        no
is_subclass_of( STRING:base, if_a) =                        no
>>> With Undefined
is_a( STRING:undefB, if_a,true) =                           no
is_a( STRING:undefB, if_a) =                                no
is_subclass_of( STRING:undefB, if_a,false) =                no
is_subclass_of( STRING:undefB, if_a) =                      no

>>> With Defined class
is_a( OBJECT:base, if_b) =                                  no
is_a( STRING:base, if_b) =                                  no
is_subclass_of( OBJECT:base, if_b) =                        no
is_subclass_of( STRING:base, if_b) =                        no
>>> With Undefined
is_a( STRING:undefB, if_b,true) =                           no
is_a( STRING:undefB, if_b) =                                no
is_subclass_of( STRING:undefB, if_b,false) =                no
is_subclass_of( STRING:undefB, if_b) =                      no

>>> With Defined class
is_a( OBJECT:base, undefA) =                                no
is_a( STRING:base, undefA) =                                no
is_subclass_of( OBJECT:base, undefA) =                      no
is_subclass_of( STRING:base, undefA) =                      no
>>> With Undefined
is_a( STRING:undefB, undefA,true) =                         no
is_a( STRING:undefB, undefA) =                              no
is_subclass_of( STRING:undefB, undefA,false) =              no
is_subclass_of( STRING:undefB, undefA) =                    no

NOW WITH AUTOLOAD


>>> With Defined class
is_a( OBJECT:base, base) =                                  yes
is_a( STRING:base, base) =                                  no
is_subclass_of( OBJECT:base, base) =                        no
is_subclass_of( STRING:base, base) =                        no
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, base,true) =                           no
is_a( STRING:undefB, base) =                                no
is_subclass_of( STRING:undefB, base,false) =                no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, base) =                      no

>>> With Defined class
is_a( OBJECT:base, derived_a) =                             no
is_a( STRING:base, derived_a) =                             no
is_subclass_of( OBJECT:base, derived_a) =                   no
is_subclass_of( STRING:base, derived_a) =                   no
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, derived_a,true) =                      no
is_a( STRING:undefB, derived_a) =                           no
is_subclass_of( STRING:undefB, derived_a,false) =           no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, derived_a) =                 no

>>> With Defined class
is_a( OBJECT:base, derived_b) =                             no
is_a( STRING:base, derived_b) =                             no
is_subclass_of( OBJECT:base, derived_b) =                   no
is_subclass_of( STRING:base, derived_b) =                   no
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, derived_b,true) =                      no
is_a( STRING:undefB, derived_b) =                           no
is_subclass_of( STRING:undefB, derived_b,false) =           no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, derived_b) =                 no

>>> With Defined class
is_a( OBJECT:base, derived_c) =                             no
is_a( STRING:base, derived_c) =                             no
is_subclass_of( OBJECT:base, derived_c) =                   no
is_subclass_of( STRING:base, derived_c) =                   no
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, derived_c,true) =                      no
is_a( STRING:undefB, derived_c) =                           no
is_subclass_of( STRING:undefB, derived_c,false) =           no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, derived_c) =                 no

>>> With Defined class
is_a( OBJECT:base, derived_d) =                             no
is_a( STRING:base, derived_d) =                             no
is_subclass_of( OBJECT:base, derived_d) =                   no
is_subclass_of( STRING:base, derived_d) =                   no
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, derived_d,true) =                      no
is_a( STRING:undefB, derived_d) =                           no
is_subclass_of( STRING:undefB, derived_d,false) =           no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, derived_d) =                 no

>>> With Defined class
is_a( OBJECT:base, if_a) =                                  no
is_a( STRING:base, if_a) =                                  no
is_subclass_of( OBJECT:base, if_a) =                        no
is_subclass_of( STRING:base, if_a) =                        no
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, if_a,true) =                           no
is_a( STRING:undefB, if_a) =                                no
is_subclass_of( STRING:undefB, if_a,false) =                no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, if_a) =                      no

>>> With Defined class
is_a( OBJECT:base, if_b) =                                  no
is_a( STRING:base, if_b) =                                  no
is_subclass_of( OBJECT:base, if_b) =                        no
is_subclass_of( STRING:base, if_b) =                        no
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, if_b,true) =                           no
is_a( STRING:undefB, if_b) =                                no
is_subclass_of( STRING:undefB, if_b,false) =                no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, if_b) =                      no

>>> With Defined class
is_a( OBJECT:base, undefA) =                                no
is_a( STRING:base, undefA) =                                no
is_subclass_of( OBJECT:base, undefA) =                      no
is_subclass_of( STRING:base, undefA) =                      no
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, undefA,true) =                         no
is_a( STRING:undefB, undefA) =                              no
is_subclass_of( STRING:undefB, undefA,false) =              no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, undefA) =                    no


>>> With Defined class
is_a( OBJECT:derived_a, base) =                             yes
is_a( STRING:derived_a, base) =                             no
is_subclass_of( OBJECT:derived_a, base) =                   yes
is_subclass_of( STRING:derived_a, base) =                   yes
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, base,true) =                           no
is_a( STRING:undefB, base) =                                no
is_subclass_of( STRING:undefB, base,false) =                no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, base) =                      no

>>> With Defined class
is_a( OBJECT:derived_a, derived_a) =                        yes
is_a( STRING:derived_a, derived_a) =                        no
is_subclass_of( OBJECT:derived_a, derived_a) =              no
is_subclass_of( STRING:derived_a, derived_a) =              no
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, derived_a,true) =                      no
is_a( STRING:undefB, derived_a) =                           no
is_subclass_of( STRING:undefB, derived_a,false) =           no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, derived_a) =                 no

>>> With Defined class
is_a( OBJECT:derived_a, derived_b) =                        no
is_a( STRING:derived_a, derived_b) =                        no
is_subclass_of( OBJECT:derived_a, derived_b) =              no
is_subclass_of( STRING:derived_a, derived_b) =              no
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, derived_b,true) =                      no
is_a( STRING:undefB, derived_b) =                           no
is_subclass_of( STRING:undefB, derived_b,false) =           no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, derived_b) =                 no

>>> With Defined class
is_a( OBJECT:derived_a, derived_c) =                        no
is_a( STRING:derived_a, derived_c) =                        no
is_subclass_of( OBJECT:derived_a, derived_c) =              no
is_subclass_of( STRING:derived_a, derived_c) =              no
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, derived_c,true) =                      no
is_a( STRING:undefB, derived_c) =                           no
is_subclass_of( STRING:undefB, derived_c,false) =           no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, derived_c) =                 no

>>> With Defined class
is_a( OBJECT:derived_a, derived_d) =                        no
is_a( STRING:derived_a, derived_d) =                        no
is_subclass_of( OBJECT:derived_a, derived_d) =              no
is_subclass_of( STRING:derived_a, derived_d) =              no
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, derived_d,true) =                      no
is_a( STRING:undefB, derived_d) =                           no
is_subclass_of( STRING:undefB, derived_d,false) =           no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, derived_d) =                 no

>>> With Defined class
is_a( OBJECT:derived_a, if_a) =                             yes
is_a( STRING:derived_a, if_a) =                             no
is_subclass_of( OBJECT:derived_a, if_a) =                   yes
is_subclass_of( STRING:derived_a, if_a) =                   yes
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, if_a,true) =                           no
is_a( STRING:undefB, if_a) =                                no
is_subclass_of( STRING:undefB, if_a,false) =                no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, if_a) =                      no

>>> With Defined class
is_a( OBJECT:derived_a, if_b) =                             no
is_a( STRING:derived_a, if_b) =                             no
is_subclass_of( OBJECT:derived_a, if_b) =                   no
is_subclass_of( STRING:derived_a, if_b) =                   no
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, if_b,true) =                           no
is_a( STRING:undefB, if_b) =                                no
is_subclass_of( STRING:undefB, if_b,false) =                no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, if_b) =                      no

>>> With Defined class
is_a( OBJECT:derived_a, undefA) =                           no
is_a( STRING:derived_a, undefA) =                           no
is_subclass_of( OBJECT:derived_a, undefA) =                 no
is_subclass_of( STRING:derived_a, undefA) =                 no
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, undefA,true) =                         no
is_a( STRING:undefB, undefA) =                              no
is_subclass_of( STRING:undefB, undefA,false) =              no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, undefA) =                    no


>>> With Defined class
is_a( OBJECT:derived_b, base) =                             yes
is_a( STRING:derived_b, base) =                             no
is_subclass_of( OBJECT:derived_b, base) =                   yes
is_subclass_of( STRING:derived_b, base) =                   yes
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, base,true) =                           no
is_a( STRING:undefB, base) =                                no
is_subclass_of( STRING:undefB, base,false) =                no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, base) =                      no

>>> With Defined class
is_a( OBJECT:derived_b, derived_a) =                        no
is_a( STRING:derived_b, derived_a) =                        no
is_subclass_of( OBJECT:derived_b, derived_a) =              no
is_subclass_of( STRING:derived_b, derived_a) =              no
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, derived_a,true) =                      no
is_a( STRING:undefB, derived_a) =                           no
is_subclass_of( STRING:undefB, derived_a,false) =           no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, derived_a) =                 no

>>> With Defined class
is_a( OBJECT:derived_b, derived_b) =                        yes
is_a( STRING:derived_b, derived_b) =                        no
is_subclass_of( OBJECT:derived_b, derived_b) =              no
is_subclass_of( STRING:derived_b, derived_b) =              no
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, derived_b,true) =                      no
is_a( STRING:undefB, derived_b) =                           no
is_subclass_of( STRING:undefB, derived_b,false) =           no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, derived_b) =                 no

>>> With Defined class
is_a( OBJECT:derived_b, derived_c) =                        no
is_a( STRING:derived_b, derived_c) =                        no
is_subclass_of( OBJECT:derived_b, derived_c) =              no
is_subclass_of( STRING:derived_b, derived_c) =              no
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, derived_c,true) =                      no
is_a( STRING:undefB, derived_c) =                           no
is_subclass_of( STRING:undefB, derived_c,false) =           no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, derived_c) =                 no

>>> With Defined class
is_a( OBJECT:derived_b, derived_d) =                        no
is_a( STRING:derived_b, derived_d) =                        no
is_subclass_of( OBJECT:derived_b, derived_d) =              no
is_subclass_of( STRING:derived_b, derived_d) =              no
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, derived_d,true) =                      no
is_a( STRING:undefB, derived_d) =                           no
is_subclass_of( STRING:undefB, derived_d,false) =           no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, derived_d) =                 no

>>> With Defined class
is_a( OBJECT:derived_b, if_a) =                             yes
is_a( STRING:derived_b, if_a) =                             no
is_subclass_of( OBJECT:derived_b, if_a) =                   yes
is_subclass_of( STRING:derived_b, if_a) =                   yes
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, if_a,true) =                           no
is_a( STRING:undefB, if_a) =                                no
is_subclass_of( STRING:undefB, if_a,false) =                no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, if_a) =                      no

>>> With Defined class
is_a( OBJECT:derived_b, if_b) =                             yes
is_a( STRING:derived_b, if_b) =                             no
is_subclass_of( OBJECT:derived_b, if_b) =                   yes
is_subclass_of( STRING:derived_b, if_b) =                   yes
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, if_b,true) =                           no
is_a( STRING:undefB, if_b) =                                no
is_subclass_of( STRING:undefB, if_b,false) =                no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, if_b) =                      no

>>> With Defined class
is_a( OBJECT:derived_b, undefA) =                           no
is_a( STRING:derived_b, undefA) =                           no
is_subclass_of( OBJECT:derived_b, undefA) =                 no
is_subclass_of( STRING:derived_b, undefA) =                 no
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, undefA,true) =                         no
is_a( STRING:undefB, undefA) =                              no
is_subclass_of( STRING:undefB, undefA,false) =              no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, undefA) =                    no


>>> With Defined class
is_a( OBJECT:derived_c, base) =                             yes
is_a( STRING:derived_c, base) =                             no
is_subclass_of( OBJECT:derived_c, base) =                   yes
is_subclass_of( STRING:derived_c, base) =                   yes
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, base,true) =                           no
is_a( STRING:undefB, base) =                                no
is_subclass_of( STRING:undefB, base,false) =                no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, base) =                      no

>>> With Defined class
is_a( OBJECT:derived_c, derived_a) =                        yes
is_a( STRING:derived_c, derived_a) =                        no
is_subclass_of( OBJECT:derived_c, derived_a) =              yes
is_subclass_of( STRING:derived_c, derived_a) =              yes
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, derived_a,true) =                      no
is_a( STRING:undefB, derived_a) =                           no
is_subclass_of( STRING:undefB, derived_a,false) =           no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, derived_a) =                 no

>>> With Defined class
is_a( OBJECT:derived_c, derived_b) =                        no
is_a( STRING:derived_c, derived_b) =                        no
is_subclass_of( OBJECT:derived_c, derived_b) =              no
is_subclass_of( STRING:derived_c, derived_b) =              no
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, derived_b,true) =                      no
is_a( STRING:undefB, derived_b) =                           no
is_subclass_of( STRING:undefB, derived_b,false) =           no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, derived_b) =                 no

>>> With Defined class
is_a( OBJECT:derived_c, derived_c) =                        yes
is_a( STRING:derived_c, derived_c) =                        no
is_subclass_of( OBJECT:derived_c, derived_c) =              no
is_subclass_of( STRING:derived_c, derived_c) =              no
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, derived_c,true) =                      no
is_a( STRING:undefB, derived_c) =                           no
is_subclass_of( STRING:undefB, derived_c,false) =           no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, derived_c) =                 no

>>> With Defined class
is_a( OBJECT:derived_c, derived_d) =                        no
is_a( STRING:derived_c, derived_d) =                        no
is_subclass_of( OBJECT:derived_c, derived_d) =              no
is_subclass_of( STRING:derived_c, derived_d) =              no
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, derived_d,true) =                      no
is_a( STRING:undefB, derived_d) =                           no
is_subclass_of( STRING:undefB, derived_d,false) =           no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, derived_d) =                 no

>>> With Defined class
is_a( OBJECT:derived_c, if_a) =                             yes
is_a( STRING:derived_c, if_a) =                             no
is_subclass_of( OBJECT:derived_c, if_a) =                   yes
is_subclass_of( STRING:derived_c, if_a) =                   yes
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, if_a,true) =                           no
is_a( STRING:undefB, if_a) =                                no
is_subclass_of( STRING:undefB, if_a,false) =                no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, if_a) =                      no

>>> With Defined class
is_a( OBJECT:derived_c, if_b) =                             yes
is_a( STRING:derived_c, if_b) =                             no
is_subclass_of( OBJECT:derived_c, if_b) =                   yes
is_subclass_of( STRING:derived_c, if_b) =                   yes
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, if_b,true) =                           no
is_a( STRING:undefB, if_b) =                                no
is_subclass_of( STRING:undefB, if_b,false) =                no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, if_b) =                      no

>>> With Defined class
is_a( OBJECT:derived_c, undefA) =                           no
is_a( STRING:derived_c, undefA) =                           no
is_subclass_of( OBJECT:derived_c, undefA) =                 no
is_subclass_of( STRING:derived_c, undefA) =                 no
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, undefA,true) =                         no
is_a( STRING:undefB, undefA) =                              no
is_subclass_of( STRING:undefB, undefA,false) =              no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, undefA) =                    no


>>> With Defined class
is_a( OBJECT:derived_d, base) =                             yes
is_a( STRING:derived_d, base) =                             no
is_subclass_of( OBJECT:derived_d, base) =                   yes
is_subclass_of( STRING:derived_d, base) =                   yes
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, base,true) =                           no
is_a( STRING:undefB, base) =                                no
is_subclass_of( STRING:undefB, base,false) =                no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, base) =                      no

>>> With Defined class
is_a( OBJECT:derived_d, derived_a) =                        yes
is_a( STRING:derived_d, derived_a) =                        no
is_subclass_of( OBJECT:derived_d, derived_a) =              yes
is_subclass_of( STRING:derived_d, derived_a) =              yes
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, derived_a,true) =                      no
is_a( STRING:undefB, derived_a) =                           no
is_subclass_of( STRING:undefB, derived_a,false) =           no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, derived_a) =                 no

>>> With Defined class
is_a( OBJECT:derived_d, derived_b) =                        no
is_a( STRING:derived_d, derived_b) =                        no
is_subclass_of( OBJECT:derived_d, derived_b) =              no
is_subclass_of( STRING:derived_d, derived_b) =              no
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, derived_b,true) =                      no
is_a( STRING:undefB, derived_b) =                           no
is_subclass_of( STRING:undefB, derived_b,false) =           no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, derived_b) =                 no

>>> With Defined class
is_a( OBJECT:derived_d, derived_c) =                        yes
is_a( STRING:derived_d, derived_c) =                        no
is_subclass_of( OBJECT:derived_d, derived_c) =              yes
is_subclass_of( STRING:derived_d, derived_c) =              yes
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, derived_c,true) =                      no
is_a( STRING:undefB, derived_c) =                           no
is_subclass_of( STRING:undefB, derived_c,false) =           no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, derived_c) =                 no

>>> With Defined class
is_a( OBJECT:derived_d, derived_d) =                        yes
is_a( STRING:derived_d, derived_d) =                        no
is_subclass_of( OBJECT:derived_d, derived_d) =              no
is_subclass_of( STRING:derived_d, derived_d) =              no
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, derived_d,true) =                      no
is_a( STRING:undefB, derived_d) =                           no
is_subclass_of( STRING:undefB, derived_d,false) =           no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, derived_d) =                 no

>>> With Defined class
is_a( OBJECT:derived_d, if_a) =                             yes
is_a( STRING:derived_d, if_a) =                             no
is_subclass_of( OBJECT:derived_d, if_a) =                   yes
is_subclass_of( STRING:derived_d, if_a) =                   yes
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, if_a,true) =                           no
is_a( STRING:undefB, if_a) =                                no
is_subclass_of( STRING:undefB, if_a,false) =                no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, if_a) =                      no

>>> With Defined class
is_a( OBJECT:derived_d, if_b) =                             yes
is_a( STRING:derived_d, if_b) =                             no
is_subclass_of( OBJECT:derived_d, if_b) =                   yes
is_subclass_of( STRING:derived_d, if_b) =                   yes
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, if_b,true) =                           no
is_a( STRING:undefB, if_b) =                                no
is_subclass_of( STRING:undefB, if_b,false) =                no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, if_b) =                      no

>>> With Defined class
is_a( OBJECT:derived_d, undefA) =                           no
is_a( STRING:derived_d, undefA) =                           no
is_subclass_of( OBJECT:derived_d, undefA) =                 no
is_subclass_of( STRING:derived_d, undefA) =                 no
>>> With Undefined
>>>> In __autoload: string(6) "undefB"
is_a( STRING:undefB, undefA,true) =                         no
is_a( STRING:undefB, undefA) =                              no
is_subclass_of( STRING:undefB, undefA,false) =              no
>>>> In __autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, undefA) =                    no
