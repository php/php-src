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
  ["APriv":"A":private]=>
  string(7) "A.APriv"
  ["AProt":protected]=>
  string(7) "A.AProt"
  ["APub"]=>
  string(6) "A.APub"
}
Serialized form:
string(98) "O:1:"A":3:{s:8:"\0A\0APriv";s:7:"A.APriv";s:8:"\0*\0AProt";s:7:"A.AProt";s:4:"APub";s:6:"A.APub";}"
Unserialized:
object(A)#%d (3) {
  ["APriv":"A":private]=>
  string(7) "A.APriv"
  ["AProt":protected]=>
  string(7) "A.AProt"
  ["APub"]=>
  string(6) "A.APub"
}
Sanity check: bool(true)


-- Test instance of B --


Before serialization:
object(B)#%d (6) {
  ["BPriv":"B":private]=>
  string(7) "B.BPriv"
  ["BProt":protected]=>
  string(7) "B.BProt"
  ["BPub"]=>
  string(6) "B.BPub"
  ["APriv":"A":private]=>
  string(7) "A.APriv"
  ["AProt":protected]=>
  string(7) "A.AProt"
  ["APub"]=>
  string(6) "A.APub"
}
Serialized form:
string(184) "O:1:"B":6:{s:8:"\0B\0BPriv";s:7:"B.BPriv";s:8:"\0*\0BProt";s:7:"B.BProt";s:4:"BPub";s:6:"B.BPub";s:8:"\0A\0APriv";s:7:"A.APriv";s:8:"\0*\0AProt";s:7:"A.AProt";s:4:"APub";s:6:"A.APub";}"
Unserialized:
object(B)#%d (6) {
  ["BPriv":"B":private]=>
  string(7) "B.BPriv"
  ["BProt":protected]=>
  string(7) "B.BProt"
  ["BPub"]=>
  string(6) "B.BPub"
  ["APriv":"A":private]=>
  string(7) "A.APriv"
  ["AProt":protected]=>
  string(7) "A.AProt"
  ["APub"]=>
  string(6) "A.APub"
}
Sanity check: bool(true)


-- Test instance of C --


Before serialization:
object(C)#%d (10) {
  ["APriv":"C":private]=>
  string(7) "C.APriv"
  ["AProt":protected]=>
  string(7) "C.AProt"
  ["APub"]=>
  string(6) "C.APub"
  ["CPriv":"C":private]=>
  string(7) "C.CPriv"
  ["CProt":protected]=>
  string(7) "C.BProt"
  ["CPub"]=>
  string(6) "C.CPub"
  ["BPriv":"B":private]=>
  string(7) "B.BPriv"
  ["BProt":protected]=>
  string(7) "B.BProt"
  ["BPub"]=>
  string(6) "B.BPub"
  ["APriv":"A":private]=>
  string(7) "A.APriv"
}
Serialized form:
string(302) "O:1:"C":10:{s:8:"\0C\0APriv";s:7:"C.APriv";s:8:"\0*\0AProt";s:7:"C.AProt";s:4:"APub";s:6:"C.APub";s:8:"\0C\0CPriv";s:7:"C.CPriv";s:8:"\0*\0CProt";s:7:"C.BProt";s:4:"CPub";s:6:"C.CPub";s:8:"\0B\0BPriv";s:7:"B.BPriv";s:8:"\0*\0BProt";s:7:"B.BProt";s:4:"BPub";s:6:"B.BPub";s:8:"\0A\0APriv";s:7:"A.APriv";}"
Unserialized:
object(C)#%d (10) {
  ["APriv":"C":private]=>
  string(7) "C.APriv"
  ["AProt":protected]=>
  string(7) "C.AProt"
  ["APub"]=>
  string(6) "C.APub"
  ["CPriv":"C":private]=>
  string(7) "C.CPriv"
  ["CProt":protected]=>
  string(7) "C.BProt"
  ["CPub"]=>
  string(6) "C.CPub"
  ["BPriv":"B":private]=>
  string(7) "B.BPriv"
  ["BProt":protected]=>
  string(7) "B.BProt"
  ["BPub"]=>
  string(6) "B.BPub"
  ["APriv":"A":private]=>
  string(7) "A.APriv"
}
Sanity check: bool(true)
Done