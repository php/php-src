--TEST--
get_object_vars(): visibility from static methods (target object passed as arg)
--FILE--
<?php
/* Prototype  : proto array get_object_vars(object obj)
 * Description: Returns an array of object properties 
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions: 
 */

Class A {
	private $hiddenPriv = 'A::hiddenPriv';

	public static function test($b) {
		echo __METHOD__ . "\n"; 
		var_dump(get_object_vars($b));
	}
}

Class B extends A {
	private $hiddenPriv = 'B::hiddenPriv';	
	private $priv = 'B::priv';
	protected $prot = 'B::prot';
	public $pub = 'B::pub';

	public static function test($b) {
		echo __METHOD__ . "\n";		
		var_dump(get_object_vars($b));
	} 
}

Class C extends B {
	private $hiddenPriv = 'C::hiddenPriv';
	
	public static function test($b) {
		echo __METHOD__ . "\n";		
		var_dump(get_object_vars($b));
	} 
}

Class X {
	public static function test($b) {
		echo __METHOD__ . "\n";		
		var_dump(get_object_vars($b));
	} 
}


$b = new B;
echo "\n---( Global scope: )---\n";
var_dump(get_object_vars($b));
echo "\n---( Declaring class: )---\n";
B::test($b);
echo "\n---( Subclass: )---\n";
C::test($b);
echo "\n---( Superclass: )---\n";
A::test($b);
echo "\n---( Unrelated class: )---\n";
X::test($b);
?>
--EXPECT--

---( Global scope: )---
array(1) {
  [u"pub"]=>
  unicode(6) "B::pub"
}

---( Declaring class: )---
B::test
array(4) {
  [u"hiddenPriv"]=>
  unicode(13) "B::hiddenPriv"
  [u"priv"]=>
  unicode(7) "B::priv"
  [u"prot"]=>
  unicode(7) "B::prot"
  [u"pub"]=>
  unicode(6) "B::pub"
}

---( Subclass: )---
C::test
array(2) {
  [u"prot"]=>
  unicode(7) "B::prot"
  [u"pub"]=>
  unicode(6) "B::pub"
}

---( Superclass: )---
A::test
array(3) {
  [u"prot"]=>
  unicode(7) "B::prot"
  [u"pub"]=>
  unicode(6) "B::pub"
  [u"hiddenPriv"]=>
  unicode(13) "A::hiddenPriv"
}

---( Unrelated class: )---
X::test
array(1) {
  [u"pub"]=>
  unicode(6) "B::pub"
}
