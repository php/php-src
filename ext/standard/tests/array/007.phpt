--TEST--
Test array_diff and array_diff_assoc behaviour
--FILE--
<?php
$a = array(1,"big"=>2,3,6,3,5,3,3,3,3,3,3,3,3,3,3);
$b = array(2,2,3,3,3,3,3,3,3,3,3,3,3,3,3);
$c = array(-1,1);
echo '$a='.var_export($a,TRUE).";\n";
echo '$b='.var_export($b,TRUE).";\n";
echo '$c='.var_export($c,TRUE).";\n";
var_dump(array_diff($a,$b,$c));
var_dump(array_diff_assoc($a,$b,$c));
$a = array(
'a'=>2,
'b'=>'some',
'c'=>'done',
'z'=>'foo',
'f'=>5,
'fan'=>'fen',
7=>18,
9=>25,
11=>42,
12=>42,
45=>42,
73=>'foo',
95=>'some',
'som3'=>'some',
'want'=>'wanna');
$b = array(
'a'=>7,
7=>18,
9=>13,
11=>42,
45=>46,
'som3'=>'some',
'foo'=>'some',
'goo'=>'foo',
'f'=>5,
'z'=>'equal'
);
$c = array(
73=>'foo',
95=>'some');
echo '$a='.var_export($a,TRUE).";\n";
echo '$b='.var_export($b,TRUE).";\n";
echo '$c='.var_export($c,TRUE).";\n";
echo "Results:\n\n";
var_dump(array_diff($a,$b,$c));
var_dump(array_diff_assoc($a,$b,$c));

echo "-=-=-=-=-=-=-=-=- New functionality from 5.0.0 -=-=-=-=-=-=-=-\n";
error_reporting(E_ALL);
class cr {
	private $priv_member;
	public  $public_member;
	function cr($val) {
		$this->priv_member = $val;
		$this->public_member = $val;
	}
	static function comp_func_cr($a, $b) {
		if ($a->priv_member === $b->priv_member) return 0;
		return ($a->priv_member > $b->priv_member)? 1:-1;
	}

	function __toString() {
		return "Object";
	}
}

function comp_func($a, $b) {
	if ($a === $b) return 0;
	return ($a > $b)? 1:-1;

}

function comp_func_cr($a, $b) {
	if ($a->public_member === $b->public_member) return 0;
	return ($a->public_member > $b->public_member)? 1:-1;
}


/*
$a = array(1,"big"=>2,3,6,3,5,3,3,3,3,3,3,3,3,3,3);
$b = array(2,2,3,3,3,3,3,3,3,3,3,3,3,3,3);
$c = array(-1,1);
echo '$a='.var_export($a,TRUE).";\n";
echo '$b='.var_export($b,TRUE).";\n";
echo '$c='.var_export($c,TRUE).";\n";
var_dump(array_diff($a,$b,$c));
var_dump(array_diff_assoc($a,$b,$c));
var_dump(array_udiff($a, $b, $c, "comp_func"));
var_dump(array_diff_uassoc($a,$b,$c, "comp_func"));
*/

/*
 $a = array(new cr(9),new cr(12),new cr(23),new cr(4),new cr(-15),);
 $b = array(new cr(9),new cr(22),new cr( 3),new cr(4),new cr(-15),);
 var_dump(array_udiff($a, $b, "comp_func_cr"));
*/
$a = array("0.1" => new cr(9), "0.5" => new cr(12), 0 => new cr(23), 1=> new cr(4), 2 => new cr(-15),);
$b = array("0.2" => new cr(9), "0.5" => new cr(22), 0 => new cr( 3), 1=> new cr(4), 2 => new cr(-15),);

echo '$a='.var_export($a,TRUE).";\n";
echo '$b='.var_export($b,TRUE).";\n";
echo 'var_dump(array_udiff_uassoc($a, $b, "comp_func_cr", "comp_func"));'."\n";
var_dump(array_udiff_uassoc($a, $b, "comp_func_cr", "comp_func"));


echo '$a='.var_export($a,TRUE).";\n";
echo '$b='.var_export($b,TRUE).";\n";
echo 'var_dump(array_udiff_uassoc($a, $b, array("cr", "comp_func_cr"), "comp_func"));'."\n";
var_dump(array_udiff_uassoc($a, $b, array("cr", "comp_func_cr"), "comp_func"));


echo '$a='.var_export($a,TRUE).";\n";
echo '$b='.var_export($b,TRUE).";\n";
echo 'var_dump(array_diff_assoc($a, $b));'."\n";
var_dump(@array_diff_assoc($a, $b));


echo '$a='.var_export($a,TRUE).";\n";
echo '$b='.var_export($b,TRUE).";\n";
echo 'var_dump(array_udiff($a, $b, "comp_func_cr"));'."\n";
var_dump(array_udiff($a, $b, "comp_func_cr"));


echo '$a='.var_export($a,TRUE).";\n";
echo '$b='.var_export($b,TRUE).";\n";
echo 'var_dump(array_udiff_assoc($a, $b, "comp_func_cr"));'."\n";
var_dump(array_udiff_assoc($a, $b, "comp_func_cr"));

?>
--EXPECTF--
$a=array (
  0 => 1,
  'big' => 2,
  1 => 3,
  2 => 6,
  3 => 3,
  4 => 5,
  5 => 3,
  6 => 3,
  7 => 3,
  8 => 3,
  9 => 3,
  10 => 3,
  11 => 3,
  12 => 3,
  13 => 3,
  14 => 3,
);
$b=array (
  0 => 2,
  1 => 2,
  2 => 3,
  3 => 3,
  4 => 3,
  5 => 3,
  6 => 3,
  7 => 3,
  8 => 3,
  9 => 3,
  10 => 3,
  11 => 3,
  12 => 3,
  13 => 3,
  14 => 3,
);
$c=array (
  0 => -1,
  1 => 1,
);
array(2) {
  [2]=>
  int(6)
  [4]=>
  int(5)
}
array(5) {
  [0]=>
  int(1)
  ["big"]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  int(6)
  [4]=>
  int(5)
}
$a=array (
  'a' => 2,
  'b' => 'some',
  'c' => 'done',
  'z' => 'foo',
  'f' => 5,
  'fan' => 'fen',
  7 => 18,
  9 => 25,
  11 => 42,
  12 => 42,
  45 => 42,
  73 => 'foo',
  95 => 'some',
  'som3' => 'some',
  'want' => 'wanna',
);
$b=array (
  'a' => 7,
  7 => 18,
  9 => 13,
  11 => 42,
  45 => 46,
  'som3' => 'some',
  'foo' => 'some',
  'goo' => 'foo',
  'f' => 5,
  'z' => 'equal',
);
$c=array (
  73 => 'foo',
  95 => 'some',
);
Results:

array(5) {
  ["a"]=>
  int(2)
  ["c"]=>
  string(4) "done"
  ["fan"]=>
  string(3) "fen"
  [9]=>
  int(25)
  ["want"]=>
  string(5) "wanna"
}
array(9) {
  ["a"]=>
  int(2)
  ["b"]=>
  string(4) "some"
  ["c"]=>
  string(4) "done"
  ["z"]=>
  string(3) "foo"
  ["fan"]=>
  string(3) "fen"
  [9]=>
  int(25)
  [12]=>
  int(42)
  [45]=>
  int(42)
  ["want"]=>
  string(5) "wanna"
}
-=-=-=-=-=-=-=-=- New functionality from 5.0.0 -=-=-=-=-=-=-=-
$a=array (
  '0.1' => 
  cr::__set_state(array(
     'priv_member' => 9,
     'public_member' => 9,
  )),
  '0.5' => 
  cr::__set_state(array(
     'priv_member' => 12,
     'public_member' => 12,
  )),
  0 => 
  cr::__set_state(array(
     'priv_member' => 23,
     'public_member' => 23,
  )),
  1 => 
  cr::__set_state(array(
     'priv_member' => 4,
     'public_member' => 4,
  )),
  2 => 
  cr::__set_state(array(
     'priv_member' => -15,
     'public_member' => -15,
  )),
);
$b=array (
  '0.2' => 
  cr::__set_state(array(
     'priv_member' => 9,
     'public_member' => 9,
  )),
  '0.5' => 
  cr::__set_state(array(
     'priv_member' => 22,
     'public_member' => 22,
  )),
  0 => 
  cr::__set_state(array(
     'priv_member' => 3,
     'public_member' => 3,
  )),
  1 => 
  cr::__set_state(array(
     'priv_member' => 4,
     'public_member' => 4,
  )),
  2 => 
  cr::__set_state(array(
     'priv_member' => -15,
     'public_member' => -15,
  )),
);
var_dump(array_udiff_uassoc($a, $b, "comp_func_cr", "comp_func"));
array(3) {
  ["0.1"]=>
  object(cr)#%d (2) {
    ["priv_member:private"]=>
    int(9)
    ["public_member"]=>
    int(9)
  }
  ["0.5"]=>
  object(cr)#%d (2) {
    ["priv_member:private"]=>
    int(12)
    ["public_member"]=>
    int(12)
  }
  [0]=>
  object(cr)#%d (2) {
    ["priv_member:private"]=>
    int(23)
    ["public_member"]=>
    int(23)
  }
}
$a=array (
  '0.1' => 
  cr::__set_state(array(
     'priv_member' => 9,
     'public_member' => 9,
  )),
  '0.5' => 
  cr::__set_state(array(
     'priv_member' => 12,
     'public_member' => 12,
  )),
  0 => 
  cr::__set_state(array(
     'priv_member' => 23,
     'public_member' => 23,
  )),
  1 => 
  cr::__set_state(array(
     'priv_member' => 4,
     'public_member' => 4,
  )),
  2 => 
  cr::__set_state(array(
     'priv_member' => -15,
     'public_member' => -15,
  )),
);
$b=array (
  '0.2' => 
  cr::__set_state(array(
     'priv_member' => 9,
     'public_member' => 9,
  )),
  '0.5' => 
  cr::__set_state(array(
     'priv_member' => 22,
     'public_member' => 22,
  )),
  0 => 
  cr::__set_state(array(
     'priv_member' => 3,
     'public_member' => 3,
  )),
  1 => 
  cr::__set_state(array(
     'priv_member' => 4,
     'public_member' => 4,
  )),
  2 => 
  cr::__set_state(array(
     'priv_member' => -15,
     'public_member' => -15,
  )),
);
var_dump(array_udiff_uassoc($a, $b, array("cr", "comp_func_cr"), "comp_func"));
array(3) {
  ["0.1"]=>
  object(cr)#%d (2) {
    ["priv_member:private"]=>
    int(9)
    ["public_member"]=>
    int(9)
  }
  ["0.5"]=>
  object(cr)#%d (2) {
    ["priv_member:private"]=>
    int(12)
    ["public_member"]=>
    int(12)
  }
  [0]=>
  object(cr)#%d (2) {
    ["priv_member:private"]=>
    int(23)
    ["public_member"]=>
    int(23)
  }
}
$a=array (
  '0.1' => 
  cr::__set_state(array(
     'priv_member' => 9,
     'public_member' => 9,
  )),
  '0.5' => 
  cr::__set_state(array(
     'priv_member' => 12,
     'public_member' => 12,
  )),
  0 => 
  cr::__set_state(array(
     'priv_member' => 23,
     'public_member' => 23,
  )),
  1 => 
  cr::__set_state(array(
     'priv_member' => 4,
     'public_member' => 4,
  )),
  2 => 
  cr::__set_state(array(
     'priv_member' => -15,
     'public_member' => -15,
  )),
);
$b=array (
  '0.2' => 
  cr::__set_state(array(
     'priv_member' => 9,
     'public_member' => 9,
  )),
  '0.5' => 
  cr::__set_state(array(
     'priv_member' => 22,
     'public_member' => 22,
  )),
  0 => 
  cr::__set_state(array(
     'priv_member' => 3,
     'public_member' => 3,
  )),
  1 => 
  cr::__set_state(array(
     'priv_member' => 4,
     'public_member' => 4,
  )),
  2 => 
  cr::__set_state(array(
     'priv_member' => -15,
     'public_member' => -15,
  )),
);
var_dump(array_diff_assoc($a, $b));
array(1) {
  ["0.1"]=>
  object(cr)#%d (2) {
    ["priv_member:private"]=>
    int(9)
    ["public_member"]=>
    int(9)
  }
}
$a=array (
  '0.1' => 
  cr::__set_state(array(
     'priv_member' => 9,
     'public_member' => 9,
  )),
  '0.5' => 
  cr::__set_state(array(
     'priv_member' => 12,
     'public_member' => 12,
  )),
  0 => 
  cr::__set_state(array(
     'priv_member' => 23,
     'public_member' => 23,
  )),
  1 => 
  cr::__set_state(array(
     'priv_member' => 4,
     'public_member' => 4,
  )),
  2 => 
  cr::__set_state(array(
     'priv_member' => -15,
     'public_member' => -15,
  )),
);
$b=array (
  '0.2' => 
  cr::__set_state(array(
     'priv_member' => 9,
     'public_member' => 9,
  )),
  '0.5' => 
  cr::__set_state(array(
     'priv_member' => 22,
     'public_member' => 22,
  )),
  0 => 
  cr::__set_state(array(
     'priv_member' => 3,
     'public_member' => 3,
  )),
  1 => 
  cr::__set_state(array(
     'priv_member' => 4,
     'public_member' => 4,
  )),
  2 => 
  cr::__set_state(array(
     'priv_member' => -15,
     'public_member' => -15,
  )),
);
var_dump(array_udiff($a, $b, "comp_func_cr"));
array(2) {
  ["0.5"]=>
  object(cr)#%d (2) {
    ["priv_member:private"]=>
    int(12)
    ["public_member"]=>
    int(12)
  }
  [0]=>
  object(cr)#%d (2) {
    ["priv_member:private"]=>
    int(23)
    ["public_member"]=>
    int(23)
  }
}
$a=array (
  '0.1' => 
  cr::__set_state(array(
     'priv_member' => 9,
     'public_member' => 9,
  )),
  '0.5' => 
  cr::__set_state(array(
     'priv_member' => 12,
     'public_member' => 12,
  )),
  0 => 
  cr::__set_state(array(
     'priv_member' => 23,
     'public_member' => 23,
  )),
  1 => 
  cr::__set_state(array(
     'priv_member' => 4,
     'public_member' => 4,
  )),
  2 => 
  cr::__set_state(array(
     'priv_member' => -15,
     'public_member' => -15,
  )),
);
$b=array (
  '0.2' => 
  cr::__set_state(array(
     'priv_member' => 9,
     'public_member' => 9,
  )),
  '0.5' => 
  cr::__set_state(array(
     'priv_member' => 22,
     'public_member' => 22,
  )),
  0 => 
  cr::__set_state(array(
     'priv_member' => 3,
     'public_member' => 3,
  )),
  1 => 
  cr::__set_state(array(
     'priv_member' => 4,
     'public_member' => 4,
  )),
  2 => 
  cr::__set_state(array(
     'priv_member' => -15,
     'public_member' => -15,
  )),
);
var_dump(array_udiff_assoc($a, $b, "comp_func_cr"));
array(3) {
  ["0.1"]=>
  object(cr)#%d (2) {
    ["priv_member:private"]=>
    int(9)
    ["public_member"]=>
    int(9)
  }
  ["0.5"]=>
  object(cr)#%d (2) {
    ["priv_member:private"]=>
    int(12)
    ["public_member"]=>
    int(12)
  }
  [0]=>
  object(cr)#%d (2) {
    ["priv_member:private"]=>
    int(23)
    ["public_member"]=>
    int(23)
  }
}
