--TEST--
Object serialization / unserialization with inherited and hidden properties.
--FILE--
<?php
/* Prototype  : proto string serialize(mixed variable)
 * Description: Returns a string representation of variable (which can later be unserialized) 
 * Source code: ext/standard/var.c
 * Alias to functions: 
 */
/* Prototype  : proto mixed unserialize(string variable_representation)
 * Description: Takes a string representation of variable and recreates it 
 * Source code: ext/standard/var.c
 * Alias to functions: 
 */

Class A {
	private $APriv = "A.APriv";
	protected $AProt = "A.AProt";
	public $APub = "A.APub";
	
	function audit() {
		return isset($this->APriv, $this->AProt, $this->APub); 
	}		
}

Class B extends A {
	private $BPriv = "B.BPriv";
	protected $BProt = "B.BProt";
	public $BPub = "B.BPub";
	
	function audit() {
		return  parent::audit() && isset($this->AProt, $this->APub,
					 $this->BPriv, $this->BProt, $this->BPub); 
	}	
}

Class C extends B {
	private $APriv = "C.APriv";
	protected $AProt = "C.AProt";
	public $APub = "C.APub";
	
	private $CPriv = "C.CPriv";
	protected $CProt = "C.BProt";
	public $CPub = "C.CPub";
	
	function audit() {
		return parent::audit() && isset($this->APriv, $this->AProt, $this->APub, 
					 $this->BProt, $this->BPub, 
					 $this->CPriv, $this->CProt, $this->CPub); 
	}
}

function prettyPrint($obj) {
	echo "\n\nBefore serialization:\n";
	var_dump($obj);

	echo "Serialized form:\n";
	$ser = serialize($obj);
	$serPrintable = str_replace("\0", '\0', $ser);
	var_dump($serPrintable);
	
	echo "Unserialized:\n";
	$uobj = unserialize($ser);
	var_dump($uobj);
	
	echo "Sanity check: ";
	var_dump($uobj->audit());
}

echo "-- Test instance of A --\n";
prettyPrint(new A);
echo "\n\n-- Test instance of B --\n";
prettyPrint(new B);
echo "\n\n-- Test instance of C --\n";
prettyPrint(new C);

echo "Done";
?>
--EXPECTF--
-- Test instance of A --


Before serialization:
object(A)#%d (3) {
  [u"APriv":u"A":private]=>
  unicode(7) "A.APriv"
  [u"AProt":protected]=>
  unicode(7) "A.AProt"
  [u"APub"]=>
  unicode(6) "A.APub"
}
Serialized form:
unicode(98) "O:1:"A":3:{U:8:"\0A\0APriv";U:7:"A.APriv";U:8:"\0*\0AProt";U:7:"A.AProt";U:4:"APub";U:6:"A.APub";}"
Unserialized:
object(A)#%d (3) {
  [u"APriv":u"A":private]=>
  unicode(7) "A.APriv"
  [u"AProt":protected]=>
  unicode(7) "A.AProt"
  [u"APub"]=>
  unicode(6) "A.APub"
}
Sanity check: bool(true)


-- Test instance of B --


Before serialization:
object(B)#%d (6) {
  [u"BPriv":u"B":private]=>
  unicode(7) "B.BPriv"
  [u"BProt":protected]=>
  unicode(7) "B.BProt"
  [u"BPub"]=>
  unicode(6) "B.BPub"
  [u"APriv":u"A":private]=>
  unicode(7) "A.APriv"
  [u"AProt":protected]=>
  unicode(7) "A.AProt"
  [u"APub"]=>
  unicode(6) "A.APub"
}
Serialized form:
unicode(184) "O:1:"B":6:{U:8:"\0B\0BPriv";U:7:"B.BPriv";U:8:"\0*\0BProt";U:7:"B.BProt";U:4:"BPub";U:6:"B.BPub";U:8:"\0A\0APriv";U:7:"A.APriv";U:8:"\0*\0AProt";U:7:"A.AProt";U:4:"APub";U:6:"A.APub";}"
Unserialized:
object(B)#%d (6) {
  [u"BPriv":u"B":private]=>
  unicode(7) "B.BPriv"
  [u"BProt":protected]=>
  unicode(7) "B.BProt"
  [u"BPub"]=>
  unicode(6) "B.BPub"
  [u"APriv":u"A":private]=>
  unicode(7) "A.APriv"
  [u"AProt":protected]=>
  unicode(7) "A.AProt"
  [u"APub"]=>
  unicode(6) "A.APub"
}
Sanity check: bool(true)


-- Test instance of C --


Before serialization:
object(C)#%d (10) {
  [u"APriv":u"C":private]=>
  unicode(7) "C.APriv"
  [u"AProt":protected]=>
  unicode(7) "C.AProt"
  [u"APub"]=>
  unicode(6) "C.APub"
  [u"CPriv":u"C":private]=>
  unicode(7) "C.CPriv"
  [u"CProt":protected]=>
  unicode(7) "C.BProt"
  [u"CPub"]=>
  unicode(6) "C.CPub"
  [u"BPriv":u"B":private]=>
  unicode(7) "B.BPriv"
  [u"BProt":protected]=>
  unicode(7) "B.BProt"
  [u"BPub"]=>
  unicode(6) "B.BPub"
  [u"APriv":u"A":private]=>
  unicode(7) "A.APriv"
}
Serialized form:
unicode(302) "O:1:"C":10:{U:8:"\0C\0APriv";U:7:"C.APriv";U:8:"\0*\0AProt";U:7:"C.AProt";U:4:"APub";U:6:"C.APub";U:8:"\0C\0CPriv";U:7:"C.CPriv";U:8:"\0*\0CProt";U:7:"C.BProt";U:4:"CPub";U:6:"C.CPub";U:8:"\0B\0BPriv";U:7:"B.BPriv";U:8:"\0*\0BProt";U:7:"B.BProt";U:4:"BPub";U:6:"B.BPub";U:8:"\0A\0APriv";U:7:"A.APriv";}"
Unserialized:
object(C)#%d (10) {
  [u"APriv":u"C":private]=>
  unicode(7) "C.APriv"
  [u"AProt":protected]=>
  unicode(7) "C.AProt"
  [u"APub"]=>
  unicode(6) "C.APub"
  [u"CPriv":u"C":private]=>
  unicode(7) "C.CPriv"
  [u"CProt":protected]=>
  unicode(7) "C.BProt"
  [u"CPub"]=>
  unicode(6) "C.CPub"
  [u"BPriv":u"B":private]=>
  unicode(7) "B.BPriv"
  [u"BProt":protected]=>
  unicode(7) "B.BProt"
  [u"BPub"]=>
  unicode(6) "B.BPub"
  [u"APriv":u"A":private]=>
  unicode(7) "A.APriv"
}
Sanity check: bool(true)
Done
