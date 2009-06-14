--TEST--
Foreach loop tests - visibility.
--FILE--
<?php

class C {
	public $a = "Original a";
	public $b = "Original b";
	public $c = "Original c";
	protected $d = "Original d";
	private $e = "Original e";

	function doForEachC() {
		echo "in C::doForEachC\n";
		foreach ($this as $k=>&$v) {
			var_dump($v);
			$v="changed.$k";
		}
	}
		
	static function doForEach($obj) {
		echo "in C::doForEach\n";
		foreach ($obj as $k=>&$v) {
			var_dump($v);
			$v="changed.$k";
		}
	}
	
	function doForEachOnThis() {
		echo "in C::doForEachOnThis\n";
		foreach ($this as $k=>&$v) {
			var_dump($v);
			$v="changed.$k";
		}
	}
	
}

class D extends C {
	
	private $f = "Original f";
	protected $g = "Original g";
	
	static function doForEach($obj) {
		echo "in D::doForEach\n";
		foreach ($obj as $k=>&$v) {
			var_dump($v);
			$v="changed.$k";
		}
	}

	function doForEachOnThis() {
		echo "in D::doForEachOnThis\n";
		foreach ($this as $k=>&$v) {
			var_dump($v);
			$v="changed.$k";
		}
	}
}

class E extends D {
	public $a = "Overridden a";
	public $b = "Overridden b";
	public $c = "Overridden c";
	protected $d = "Overridden d";
	private $e = "Overridden e";	

	static function doForEach($obj) {
		echo "in E::doForEach\n";
		foreach ($obj as $k=>&$v) {
			var_dump($v);
			$v="changed.$k";
		}
	}

	function doForEachOnThis() {
		echo "in E::doForEachOnThis\n";
		foreach ($this as $k=>&$v) {
			var_dump($v);
			$v="changed.$k";
		}
	}
}

echo "\n\nIterate over various generations from within overridden methods:\n";
echo "\n--> Using instance of C:\n";
$myC = new C;
$myC->doForEachOnThis();
var_dump($myC);
echo "\n--> Using instance of D:\n";
$myD = new D;
$myD->doForEachOnThis();
var_dump($myD);
echo "\n--> Using instance of E:\n";
$myE = new E;
$myE->doForEachOnThis();
var_dump($myE);

echo "\n\nIterate over various generations from within an inherited method:\n";
echo "\n--> Using instance of C:\n";
$myC = new C;
$myC->doForEachC();
var_dump($myC);
echo "\n--> Using instance of D:\n";
$myD = new D;
$myD->doForEachC();
var_dump($myD);
echo "\n--> Using instance of E:\n";
$myE = new E;
$myE->doForEachC();
var_dump($myE);

echo "\n\nIterate over various generations from within an overridden static method:\n";
echo "\n--> Using instance of C:\n";
$myC = new C;
C::doForEach($myC);
var_dump($myC);
$myC = new C;
D::doForEach($myC);
var_dump($myC);
$myC = new C;
E::doForEach($myC);
var_dump($myC);
echo "\n--> Using instance of D:\n";
$myD = new D;
C::doForEach($myD);
var_dump($myD);
$myD = new D;
D::doForEach($myD);
var_dump($myD);
$myD = new D;
E::doForEach($myD);
var_dump($myD);
echo "\n--> Using instance of E:\n";
$myE = new E;
C::doForEach($myE);
var_dump($myE);
$myE = new E;
D::doForEach($myE);
var_dump($myE);
$myE = new E;
E::doForEach($myE);
var_dump($myE);


echo "\n\nIterate over various generations from outside the object:\n";
echo "\n--> Using instance of C:\n";
$myC = new C;
foreach ($myC as $k=>&$v) {
	var_dump($v);
	$v="changed.$k";
}
var_dump($myC);
echo "\n--> Using instance of D:\n";
$myD = new D;
foreach ($myD as $k=>&$v) {
	var_dump($v);
	$v="changed.$k";
}
var_dump($myD);
echo "\n--> Using instance of E:\n";
$myE = new E;
foreach ($myE as $k=>&$v) {
	var_dump($v);
	$v="changed.$k";
}
var_dump($myE);
?>
===DONE===
--EXPECTF--
Iterate over various generations from within overridden methods:

--> Using instance of C:
in C::doForEachOnThis
unicode(10) "Original a"
unicode(10) "Original b"
unicode(10) "Original c"
unicode(10) "Original d"
unicode(10) "Original e"
object(C)#%d (5) {
  [u"a"]=>
  unicode(9) "changed.a"
  [u"b"]=>
  unicode(9) "changed.b"
  [u"c"]=>
  unicode(9) "changed.c"
  [u"d":protected]=>
  unicode(9) "changed.d"
  [u"e":u"C":private]=>
  unicode(9) "changed.e"
}

--> Using instance of D:
in D::doForEachOnThis
unicode(10) "Original f"
unicode(10) "Original g"
unicode(10) "Original a"
unicode(10) "Original b"
unicode(10) "Original c"
unicode(10) "Original d"
object(D)#%d (7) {
  [u"f":u"D":private]=>
  unicode(9) "changed.f"
  [u"g":protected]=>
  unicode(9) "changed.g"
  [u"a"]=>
  unicode(9) "changed.a"
  [u"b"]=>
  unicode(9) "changed.b"
  [u"c"]=>
  unicode(9) "changed.c"
  [u"d":protected]=>
  unicode(9) "changed.d"
  [u"e":u"C":private]=>
  unicode(10) "Original e"
}

--> Using instance of E:
in E::doForEachOnThis
unicode(12) "Overridden a"
unicode(12) "Overridden b"
unicode(12) "Overridden c"
unicode(12) "Overridden d"
unicode(12) "Overridden e"
unicode(10) "Original g"
object(E)#%d (8) {
  [u"a"]=>
  unicode(9) "changed.a"
  [u"b"]=>
  unicode(9) "changed.b"
  [u"c"]=>
  unicode(9) "changed.c"
  [u"d":protected]=>
  unicode(9) "changed.d"
  [u"e":u"E":private]=>
  unicode(9) "changed.e"
  [u"f":u"D":private]=>
  unicode(10) "Original f"
  [u"g":protected]=>
  unicode(9) "changed.g"
  [u"e":u"C":private]=>
  unicode(10) "Original e"
}


Iterate over various generations from within an inherited method:

--> Using instance of C:
in C::doForEachC
unicode(10) "Original a"
unicode(10) "Original b"
unicode(10) "Original c"
unicode(10) "Original d"
unicode(10) "Original e"
object(C)#%d (5) {
  [u"a"]=>
  unicode(9) "changed.a"
  [u"b"]=>
  unicode(9) "changed.b"
  [u"c"]=>
  unicode(9) "changed.c"
  [u"d":protected]=>
  unicode(9) "changed.d"
  [u"e":u"C":private]=>
  unicode(9) "changed.e"
}

--> Using instance of D:
in C::doForEachC
unicode(10) "Original g"
unicode(10) "Original a"
unicode(10) "Original b"
unicode(10) "Original c"
unicode(10) "Original d"
unicode(10) "Original e"
object(D)#%d (7) {
  [u"f":u"D":private]=>
  unicode(10) "Original f"
  [u"g":protected]=>
  unicode(9) "changed.g"
  [u"a"]=>
  unicode(9) "changed.a"
  [u"b"]=>
  unicode(9) "changed.b"
  [u"c"]=>
  unicode(9) "changed.c"
  [u"d":protected]=>
  unicode(9) "changed.d"
  [u"e":u"C":private]=>
  unicode(9) "changed.e"
}

--> Using instance of E:
in C::doForEachC
unicode(12) "Overridden a"
unicode(12) "Overridden b"
unicode(12) "Overridden c"
unicode(12) "Overridden d"
unicode(10) "Original g"
unicode(10) "Original e"
object(E)#%d (8) {
  [u"a"]=>
  unicode(9) "changed.a"
  [u"b"]=>
  unicode(9) "changed.b"
  [u"c"]=>
  unicode(9) "changed.c"
  [u"d":protected]=>
  unicode(9) "changed.d"
  [u"e":u"E":private]=>
  unicode(12) "Overridden e"
  [u"f":u"D":private]=>
  unicode(10) "Original f"
  [u"g":protected]=>
  unicode(9) "changed.g"
  [u"e":u"C":private]=>
  unicode(9) "changed.e"
}


Iterate over various generations from within an overridden static method:

--> Using instance of C:
in C::doForEach
unicode(10) "Original a"
unicode(10) "Original b"
unicode(10) "Original c"
unicode(10) "Original d"
unicode(10) "Original e"
object(C)#%d (5) {
  [u"a"]=>
  unicode(9) "changed.a"
  [u"b"]=>
  unicode(9) "changed.b"
  [u"c"]=>
  unicode(9) "changed.c"
  [u"d":protected]=>
  unicode(9) "changed.d"
  [u"e":u"C":private]=>
  unicode(9) "changed.e"
}
in D::doForEach
unicode(10) "Original a"
unicode(10) "Original b"
unicode(10) "Original c"
unicode(10) "Original d"
object(C)#%d (5) {
  [u"a"]=>
  unicode(9) "changed.a"
  [u"b"]=>
  unicode(9) "changed.b"
  [u"c"]=>
  unicode(9) "changed.c"
  [u"d":protected]=>
  unicode(9) "changed.d"
  [u"e":u"C":private]=>
  unicode(10) "Original e"
}
in E::doForEach
unicode(10) "Original a"
unicode(10) "Original b"
unicode(10) "Original c"
unicode(10) "Original d"
object(C)#%d (5) {
  [u"a"]=>
  unicode(9) "changed.a"
  [u"b"]=>
  unicode(9) "changed.b"
  [u"c"]=>
  unicode(9) "changed.c"
  [u"d":protected]=>
  unicode(9) "changed.d"
  [u"e":u"C":private]=>
  unicode(10) "Original e"
}

--> Using instance of D:
in C::doForEach
unicode(10) "Original g"
unicode(10) "Original a"
unicode(10) "Original b"
unicode(10) "Original c"
unicode(10) "Original d"
unicode(10) "Original e"
object(D)#%d (7) {
  [u"f":u"D":private]=>
  unicode(10) "Original f"
  [u"g":protected]=>
  unicode(9) "changed.g"
  [u"a"]=>
  unicode(9) "changed.a"
  [u"b"]=>
  unicode(9) "changed.b"
  [u"c"]=>
  unicode(9) "changed.c"
  [u"d":protected]=>
  unicode(9) "changed.d"
  [u"e":u"C":private]=>
  unicode(9) "changed.e"
}
in D::doForEach
unicode(10) "Original f"
unicode(10) "Original g"
unicode(10) "Original a"
unicode(10) "Original b"
unicode(10) "Original c"
unicode(10) "Original d"
object(D)#%d (7) {
  [u"f":u"D":private]=>
  unicode(9) "changed.f"
  [u"g":protected]=>
  unicode(9) "changed.g"
  [u"a"]=>
  unicode(9) "changed.a"
  [u"b"]=>
  unicode(9) "changed.b"
  [u"c"]=>
  unicode(9) "changed.c"
  [u"d":protected]=>
  unicode(9) "changed.d"
  [u"e":u"C":private]=>
  unicode(10) "Original e"
}
in E::doForEach
unicode(10) "Original g"
unicode(10) "Original a"
unicode(10) "Original b"
unicode(10) "Original c"
unicode(10) "Original d"
object(D)#%d (7) {
  [u"f":u"D":private]=>
  unicode(10) "Original f"
  [u"g":protected]=>
  unicode(9) "changed.g"
  [u"a"]=>
  unicode(9) "changed.a"
  [u"b"]=>
  unicode(9) "changed.b"
  [u"c"]=>
  unicode(9) "changed.c"
  [u"d":protected]=>
  unicode(9) "changed.d"
  [u"e":u"C":private]=>
  unicode(10) "Original e"
}

--> Using instance of E:
in C::doForEach
unicode(12) "Overridden a"
unicode(12) "Overridden b"
unicode(12) "Overridden c"
unicode(12) "Overridden d"
unicode(10) "Original g"
unicode(10) "Original e"
object(E)#%d (8) {
  [u"a"]=>
  unicode(9) "changed.a"
  [u"b"]=>
  unicode(9) "changed.b"
  [u"c"]=>
  unicode(9) "changed.c"
  [u"d":protected]=>
  unicode(9) "changed.d"
  [u"e":u"E":private]=>
  unicode(12) "Overridden e"
  [u"f":u"D":private]=>
  unicode(10) "Original f"
  [u"g":protected]=>
  unicode(9) "changed.g"
  [u"e":u"C":private]=>
  unicode(9) "changed.e"
}
in D::doForEach
unicode(12) "Overridden a"
unicode(12) "Overridden b"
unicode(12) "Overridden c"
unicode(12) "Overridden d"
unicode(10) "Original f"
unicode(10) "Original g"
object(E)#%d (8) {
  [u"a"]=>
  unicode(9) "changed.a"
  [u"b"]=>
  unicode(9) "changed.b"
  [u"c"]=>
  unicode(9) "changed.c"
  [u"d":protected]=>
  unicode(9) "changed.d"
  [u"e":u"E":private]=>
  unicode(12) "Overridden e"
  [u"f":u"D":private]=>
  unicode(9) "changed.f"
  [u"g":protected]=>
  unicode(9) "changed.g"
  [u"e":u"C":private]=>
  unicode(10) "Original e"
}
in E::doForEach
unicode(12) "Overridden a"
unicode(12) "Overridden b"
unicode(12) "Overridden c"
unicode(12) "Overridden d"
unicode(12) "Overridden e"
unicode(10) "Original g"
object(E)#%d (8) {
  [u"a"]=>
  unicode(9) "changed.a"
  [u"b"]=>
  unicode(9) "changed.b"
  [u"c"]=>
  unicode(9) "changed.c"
  [u"d":protected]=>
  unicode(9) "changed.d"
  [u"e":u"E":private]=>
  unicode(9) "changed.e"
  [u"f":u"D":private]=>
  unicode(10) "Original f"
  [u"g":protected]=>
  unicode(9) "changed.g"
  [u"e":u"C":private]=>
  unicode(10) "Original e"
}


Iterate over various generations from outside the object:

--> Using instance of C:
unicode(10) "Original a"
unicode(10) "Original b"
unicode(10) "Original c"
object(C)#%d (5) {
  [u"a"]=>
  unicode(9) "changed.a"
  [u"b"]=>
  unicode(9) "changed.b"
  [u"c"]=>
  &unicode(9) "changed.c"
  [u"d":protected]=>
  unicode(10) "Original d"
  [u"e":u"C":private]=>
  unicode(10) "Original e"
}

--> Using instance of D:
unicode(10) "Original a"
unicode(10) "Original b"
unicode(10) "Original c"
object(D)#%d (7) {
  [u"f":u"D":private]=>
  unicode(10) "Original f"
  [u"g":protected]=>
  unicode(10) "Original g"
  [u"a"]=>
  unicode(9) "changed.a"
  [u"b"]=>
  unicode(9) "changed.b"
  [u"c"]=>
  &unicode(9) "changed.c"
  [u"d":protected]=>
  unicode(10) "Original d"
  [u"e":u"C":private]=>
  unicode(10) "Original e"
}

--> Using instance of E:
unicode(12) "Overridden a"
unicode(12) "Overridden b"
unicode(12) "Overridden c"
object(E)#%d (8) {
  [u"a"]=>
  unicode(9) "changed.a"
  [u"b"]=>
  unicode(9) "changed.b"
  [u"c"]=>
  &unicode(9) "changed.c"
  [u"d":protected]=>
  unicode(12) "Overridden d"
  [u"e":u"E":private]=>
  unicode(12) "Overridden e"
  [u"f":u"D":private]=>
  unicode(10) "Original f"
  [u"g":protected]=>
  unicode(10) "Original g"
  [u"e":u"C":private]=>
  unicode(10) "Original e"
}
===DONE===