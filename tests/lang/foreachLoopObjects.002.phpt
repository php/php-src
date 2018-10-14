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
string(10) "Original a"
string(10) "Original b"
string(10) "Original c"
string(10) "Original d"
string(10) "Original e"
object(C)#%d (5) {
  ["a"]=>
  string(9) "changed.a"
  ["b"]=>
  string(9) "changed.b"
  ["c"]=>
  string(9) "changed.c"
  ["d":protected]=>
  string(9) "changed.d"
  ["e":"C":private]=>
  string(9) "changed.e"
}

--> Using instance of D:
in D::doForEachOnThis
string(10) "Original f"
string(10) "Original g"
string(10) "Original a"
string(10) "Original b"
string(10) "Original c"
string(10) "Original d"
object(D)#%d (7) {
  ["f":"D":private]=>
  string(9) "changed.f"
  ["g":protected]=>
  string(9) "changed.g"
  ["a"]=>
  string(9) "changed.a"
  ["b"]=>
  string(9) "changed.b"
  ["c"]=>
  string(9) "changed.c"
  ["d":protected]=>
  string(9) "changed.d"
  ["e":"C":private]=>
  string(10) "Original e"
}

--> Using instance of E:
in E::doForEachOnThis
string(12) "Overridden a"
string(12) "Overridden b"
string(12) "Overridden c"
string(12) "Overridden d"
string(12) "Overridden e"
string(10) "Original g"
object(E)#%d (8) {
  ["a"]=>
  string(9) "changed.a"
  ["b"]=>
  string(9) "changed.b"
  ["c"]=>
  string(9) "changed.c"
  ["d":protected]=>
  string(9) "changed.d"
  ["e":"E":private]=>
  string(9) "changed.e"
  ["f":"D":private]=>
  string(10) "Original f"
  ["g":protected]=>
  string(9) "changed.g"
  ["e":"C":private]=>
  string(10) "Original e"
}


Iterate over various generations from within an inherited method:

--> Using instance of C:
in C::doForEachC
string(10) "Original a"
string(10) "Original b"
string(10) "Original c"
string(10) "Original d"
string(10) "Original e"
object(C)#%d (5) {
  ["a"]=>
  string(9) "changed.a"
  ["b"]=>
  string(9) "changed.b"
  ["c"]=>
  string(9) "changed.c"
  ["d":protected]=>
  string(9) "changed.d"
  ["e":"C":private]=>
  string(9) "changed.e"
}

--> Using instance of D:
in C::doForEachC
string(10) "Original g"
string(10) "Original a"
string(10) "Original b"
string(10) "Original c"
string(10) "Original d"
string(10) "Original e"
object(D)#%d (7) {
  ["f":"D":private]=>
  string(10) "Original f"
  ["g":protected]=>
  string(9) "changed.g"
  ["a"]=>
  string(9) "changed.a"
  ["b"]=>
  string(9) "changed.b"
  ["c"]=>
  string(9) "changed.c"
  ["d":protected]=>
  string(9) "changed.d"
  ["e":"C":private]=>
  string(9) "changed.e"
}

--> Using instance of E:
in C::doForEachC
string(12) "Overridden a"
string(12) "Overridden b"
string(12) "Overridden c"
string(12) "Overridden d"
string(10) "Original g"
string(10) "Original e"
object(E)#%d (8) {
  ["a"]=>
  string(9) "changed.a"
  ["b"]=>
  string(9) "changed.b"
  ["c"]=>
  string(9) "changed.c"
  ["d":protected]=>
  string(9) "changed.d"
  ["e":"E":private]=>
  string(12) "Overridden e"
  ["f":"D":private]=>
  string(10) "Original f"
  ["g":protected]=>
  string(9) "changed.g"
  ["e":"C":private]=>
  string(9) "changed.e"
}


Iterate over various generations from within an overridden static method:

--> Using instance of C:
in C::doForEach
string(10) "Original a"
string(10) "Original b"
string(10) "Original c"
string(10) "Original d"
string(10) "Original e"
object(C)#%d (5) {
  ["a"]=>
  string(9) "changed.a"
  ["b"]=>
  string(9) "changed.b"
  ["c"]=>
  string(9) "changed.c"
  ["d":protected]=>
  string(9) "changed.d"
  ["e":"C":private]=>
  string(9) "changed.e"
}
in D::doForEach
string(10) "Original a"
string(10) "Original b"
string(10) "Original c"
string(10) "Original d"
object(C)#%d (5) {
  ["a"]=>
  string(9) "changed.a"
  ["b"]=>
  string(9) "changed.b"
  ["c"]=>
  string(9) "changed.c"
  ["d":protected]=>
  string(9) "changed.d"
  ["e":"C":private]=>
  string(10) "Original e"
}
in E::doForEach
string(10) "Original a"
string(10) "Original b"
string(10) "Original c"
string(10) "Original d"
object(C)#%d (5) {
  ["a"]=>
  string(9) "changed.a"
  ["b"]=>
  string(9) "changed.b"
  ["c"]=>
  string(9) "changed.c"
  ["d":protected]=>
  string(9) "changed.d"
  ["e":"C":private]=>
  string(10) "Original e"
}

--> Using instance of D:
in C::doForEach
string(10) "Original g"
string(10) "Original a"
string(10) "Original b"
string(10) "Original c"
string(10) "Original d"
string(10) "Original e"
object(D)#%d (7) {
  ["f":"D":private]=>
  string(10) "Original f"
  ["g":protected]=>
  string(9) "changed.g"
  ["a"]=>
  string(9) "changed.a"
  ["b"]=>
  string(9) "changed.b"
  ["c"]=>
  string(9) "changed.c"
  ["d":protected]=>
  string(9) "changed.d"
  ["e":"C":private]=>
  string(9) "changed.e"
}
in D::doForEach
string(10) "Original f"
string(10) "Original g"
string(10) "Original a"
string(10) "Original b"
string(10) "Original c"
string(10) "Original d"
object(D)#%d (7) {
  ["f":"D":private]=>
  string(9) "changed.f"
  ["g":protected]=>
  string(9) "changed.g"
  ["a"]=>
  string(9) "changed.a"
  ["b"]=>
  string(9) "changed.b"
  ["c"]=>
  string(9) "changed.c"
  ["d":protected]=>
  string(9) "changed.d"
  ["e":"C":private]=>
  string(10) "Original e"
}
in E::doForEach
string(10) "Original g"
string(10) "Original a"
string(10) "Original b"
string(10) "Original c"
string(10) "Original d"
object(D)#%d (7) {
  ["f":"D":private]=>
  string(10) "Original f"
  ["g":protected]=>
  string(9) "changed.g"
  ["a"]=>
  string(9) "changed.a"
  ["b"]=>
  string(9) "changed.b"
  ["c"]=>
  string(9) "changed.c"
  ["d":protected]=>
  string(9) "changed.d"
  ["e":"C":private]=>
  string(10) "Original e"
}

--> Using instance of E:
in C::doForEach
string(12) "Overridden a"
string(12) "Overridden b"
string(12) "Overridden c"
string(12) "Overridden d"
string(10) "Original g"
string(10) "Original e"
object(E)#%d (8) {
  ["a"]=>
  string(9) "changed.a"
  ["b"]=>
  string(9) "changed.b"
  ["c"]=>
  string(9) "changed.c"
  ["d":protected]=>
  string(9) "changed.d"
  ["e":"E":private]=>
  string(12) "Overridden e"
  ["f":"D":private]=>
  string(10) "Original f"
  ["g":protected]=>
  string(9) "changed.g"
  ["e":"C":private]=>
  string(9) "changed.e"
}
in D::doForEach
string(12) "Overridden a"
string(12) "Overridden b"
string(12) "Overridden c"
string(12) "Overridden d"
string(10) "Original f"
string(10) "Original g"
object(E)#%d (8) {
  ["a"]=>
  string(9) "changed.a"
  ["b"]=>
  string(9) "changed.b"
  ["c"]=>
  string(9) "changed.c"
  ["d":protected]=>
  string(9) "changed.d"
  ["e":"E":private]=>
  string(12) "Overridden e"
  ["f":"D":private]=>
  string(9) "changed.f"
  ["g":protected]=>
  string(9) "changed.g"
  ["e":"C":private]=>
  string(10) "Original e"
}
in E::doForEach
string(12) "Overridden a"
string(12) "Overridden b"
string(12) "Overridden c"
string(12) "Overridden d"
string(12) "Overridden e"
string(10) "Original g"
object(E)#%d (8) {
  ["a"]=>
  string(9) "changed.a"
  ["b"]=>
  string(9) "changed.b"
  ["c"]=>
  string(9) "changed.c"
  ["d":protected]=>
  string(9) "changed.d"
  ["e":"E":private]=>
  string(9) "changed.e"
  ["f":"D":private]=>
  string(10) "Original f"
  ["g":protected]=>
  string(9) "changed.g"
  ["e":"C":private]=>
  string(10) "Original e"
}


Iterate over various generations from outside the object:

--> Using instance of C:
string(10) "Original a"
string(10) "Original b"
string(10) "Original c"
object(C)#%d (5) {
  ["a"]=>
  string(9) "changed.a"
  ["b"]=>
  string(9) "changed.b"
  ["c"]=>
  &string(9) "changed.c"
  ["d":protected]=>
  string(10) "Original d"
  ["e":"C":private]=>
  string(10) "Original e"
}

--> Using instance of D:
string(10) "Original a"
string(10) "Original b"
string(10) "Original c"
object(D)#%d (7) {
  ["f":"D":private]=>
  string(10) "Original f"
  ["g":protected]=>
  string(10) "Original g"
  ["a"]=>
  string(9) "changed.a"
  ["b"]=>
  string(9) "changed.b"
  ["c"]=>
  &string(9) "changed.c"
  ["d":protected]=>
  string(10) "Original d"
  ["e":"C":private]=>
  string(10) "Original e"
}

--> Using instance of E:
string(12) "Overridden a"
string(12) "Overridden b"
string(12) "Overridden c"
object(E)#%d (8) {
  ["a"]=>
  string(9) "changed.a"
  ["b"]=>
  string(9) "changed.b"
  ["c"]=>
  &string(9) "changed.c"
  ["d":protected]=>
  string(12) "Overridden d"
  ["e":"E":private]=>
  string(12) "Overridden e"
  ["f":"D":private]=>
  string(10) "Original f"
  ["g":protected]=>
  string(10) "Original g"
  ["e":"C":private]=>
  string(10) "Original e"
}
===DONE===
