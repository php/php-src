--TEST--
Test of the *intersect* bunch of functions (both assoc and non-assoc)
--FILE--
<?php
error_reporting(E_ALL);
class cr {
    private $priv_member;
    public  $public_member;
    function __construct($val) {
        $this->priv_member = $val;
        $this->public_member = $val;
    }
    static function comp_func_cr($a, $b) {
        if ($a->priv_member === $b->priv_member) return 0;
        return ($a->priv_member > $b->priv_member)? 1:-1;
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

$a = array("0.1" => new cr(9), "0.5" => new cr(12), 0 => new cr(23), 1=> new cr(4), 2 => new cr(-15),);
$b = array("0.2" => new cr(9), "0.5" => new cr(22), 0 => new cr( 3), 1=> new cr(4), 2 => new cr(-15),);

/* array_uintersect() */
echo "begin ------------ array_uintersect() ---------------------------\n";
echo '$a='.var_export($a,TRUE).";\n";
echo '$b='.var_export($b,TRUE).";\n";
echo 'var_dump(array_uintersect($a, $b, "comp_func_cr"));'."\n";
var_dump(array_uintersect($a, $b, "comp_func_cr"));
echo "end   ------------ array_uintersect() ---------------------------\n";

/* array_uintersect_assoc() */
echo "begin ------------ array_uintersect_assoc() ---------------------\n";
echo '$a='.var_export($a,TRUE).";\n";
echo '$b='.var_export($b,TRUE).";\n";
echo 'var_dump(array_uintersect_assoc($a, $b, "comp_func_cr"));'."\n";
var_dump(array_uintersect_assoc($a, $b, "comp_func_cr"));
echo "end   ------------ array_uintersect_assoc() ---------------------\n";

/* array_uintersect_uassoc() - with ordinary function */
echo "begin ------------ array_uintersect_uassoc() with ordinary func -\n";
echo '$a='.var_export($a,TRUE).";\n";
echo '$b='.var_export($b,TRUE).";\n";
echo 'var_dump(array_uintersect_uassoc($a, $b, "comp_func_cr", "comp_func"));'."\n";
var_dump(array_uintersect_uassoc($a, $b, "comp_func_cr", "comp_func"));
echo "end   ------------ array_uintersect_uassoc() with ordinary func -\n";

/* array_uintersect_uassoc() - by method call */
echo "begin ------------ array_uintersect_uassoc() with method --------\n";
echo '$a='.var_export($a,TRUE).";\n";
echo '$b='.var_export($b,TRUE).";\n";
echo 'var_dump(array_uintersect_uassoc($a, $b, array("cr", "comp_func_cr"), "comp_func"));'."\n";
var_dump(array_uintersect_uassoc($a, $b, array("cr", "comp_func_cr"), "comp_func"));
echo "end   ------------ array_uintersect_uassoc() with method --------\n";
?>
--EXPECTF--
begin ------------ array_uintersect() ---------------------------
$a=array (
  '0.1' => 
  \cr::__set_state(array(
     'priv_member' => 9,
     'public_member' => 9,
  )),
  '0.5' => 
  \cr::__set_state(array(
     'priv_member' => 12,
     'public_member' => 12,
  )),
  0 => 
  \cr::__set_state(array(
     'priv_member' => 23,
     'public_member' => 23,
  )),
  1 => 
  \cr::__set_state(array(
     'priv_member' => 4,
     'public_member' => 4,
  )),
  2 => 
  \cr::__set_state(array(
     'priv_member' => -15,
     'public_member' => -15,
  )),
);
$b=array (
  '0.2' => 
  \cr::__set_state(array(
     'priv_member' => 9,
     'public_member' => 9,
  )),
  '0.5' => 
  \cr::__set_state(array(
     'priv_member' => 22,
     'public_member' => 22,
  )),
  0 => 
  \cr::__set_state(array(
     'priv_member' => 3,
     'public_member' => 3,
  )),
  1 => 
  \cr::__set_state(array(
     'priv_member' => 4,
     'public_member' => 4,
  )),
  2 => 
  \cr::__set_state(array(
     'priv_member' => -15,
     'public_member' => -15,
  )),
);
var_dump(array_uintersect($a, $b, "comp_func_cr"));
array(3) {
  ["0.1"]=>
  object(cr)#%d (2) {
    ["priv_member":"cr":private]=>
    int(9)
    ["public_member"]=>
    int(9)
  }
  [1]=>
  object(cr)#%d (2) {
    ["priv_member":"cr":private]=>
    int(4)
    ["public_member"]=>
    int(4)
  }
  [2]=>
  object(cr)#%d (2) {
    ["priv_member":"cr":private]=>
    int(-15)
    ["public_member"]=>
    int(-15)
  }
}
end   ------------ array_uintersect() ---------------------------
begin ------------ array_uintersect_assoc() ---------------------
$a=array (
  '0.1' => 
  \cr::__set_state(array(
     'priv_member' => 9,
     'public_member' => 9,
  )),
  '0.5' => 
  \cr::__set_state(array(
     'priv_member' => 12,
     'public_member' => 12,
  )),
  0 => 
  \cr::__set_state(array(
     'priv_member' => 23,
     'public_member' => 23,
  )),
  1 => 
  \cr::__set_state(array(
     'priv_member' => 4,
     'public_member' => 4,
  )),
  2 => 
  \cr::__set_state(array(
     'priv_member' => -15,
     'public_member' => -15,
  )),
);
$b=array (
  '0.2' => 
  \cr::__set_state(array(
     'priv_member' => 9,
     'public_member' => 9,
  )),
  '0.5' => 
  \cr::__set_state(array(
     'priv_member' => 22,
     'public_member' => 22,
  )),
  0 => 
  \cr::__set_state(array(
     'priv_member' => 3,
     'public_member' => 3,
  )),
  1 => 
  \cr::__set_state(array(
     'priv_member' => 4,
     'public_member' => 4,
  )),
  2 => 
  \cr::__set_state(array(
     'priv_member' => -15,
     'public_member' => -15,
  )),
);
var_dump(array_uintersect_assoc($a, $b, "comp_func_cr"));
array(2) {
  [1]=>
  object(cr)#%d (2) {
    ["priv_member":"cr":private]=>
    int(4)
    ["public_member"]=>
    int(4)
  }
  [2]=>
  object(cr)#%d (2) {
    ["priv_member":"cr":private]=>
    int(-15)
    ["public_member"]=>
    int(-15)
  }
}
end   ------------ array_uintersect_assoc() ---------------------
begin ------------ array_uintersect_uassoc() with ordinary func -
$a=array (
  '0.1' => 
  \cr::__set_state(array(
     'priv_member' => 9,
     'public_member' => 9,
  )),
  '0.5' => 
  \cr::__set_state(array(
     'priv_member' => 12,
     'public_member' => 12,
  )),
  0 => 
  \cr::__set_state(array(
     'priv_member' => 23,
     'public_member' => 23,
  )),
  1 => 
  \cr::__set_state(array(
     'priv_member' => 4,
     'public_member' => 4,
  )),
  2 => 
  \cr::__set_state(array(
     'priv_member' => -15,
     'public_member' => -15,
  )),
);
$b=array (
  '0.2' => 
  \cr::__set_state(array(
     'priv_member' => 9,
     'public_member' => 9,
  )),
  '0.5' => 
  \cr::__set_state(array(
     'priv_member' => 22,
     'public_member' => 22,
  )),
  0 => 
  \cr::__set_state(array(
     'priv_member' => 3,
     'public_member' => 3,
  )),
  1 => 
  \cr::__set_state(array(
     'priv_member' => 4,
     'public_member' => 4,
  )),
  2 => 
  \cr::__set_state(array(
     'priv_member' => -15,
     'public_member' => -15,
  )),
);
var_dump(array_uintersect_uassoc($a, $b, "comp_func_cr", "comp_func"));
array(2) {
  [1]=>
  object(cr)#%d (2) {
    ["priv_member":"cr":private]=>
    int(4)
    ["public_member"]=>
    int(4)
  }
  [2]=>
  object(cr)#%d (2) {
    ["priv_member":"cr":private]=>
    int(-15)
    ["public_member"]=>
    int(-15)
  }
}
end   ------------ array_uintersect_uassoc() with ordinary func -
begin ------------ array_uintersect_uassoc() with method --------
$a=array (
  '0.1' => 
  \cr::__set_state(array(
     'priv_member' => 9,
     'public_member' => 9,
  )),
  '0.5' => 
  \cr::__set_state(array(
     'priv_member' => 12,
     'public_member' => 12,
  )),
  0 => 
  \cr::__set_state(array(
     'priv_member' => 23,
     'public_member' => 23,
  )),
  1 => 
  \cr::__set_state(array(
     'priv_member' => 4,
     'public_member' => 4,
  )),
  2 => 
  \cr::__set_state(array(
     'priv_member' => -15,
     'public_member' => -15,
  )),
);
$b=array (
  '0.2' => 
  \cr::__set_state(array(
     'priv_member' => 9,
     'public_member' => 9,
  )),
  '0.5' => 
  \cr::__set_state(array(
     'priv_member' => 22,
     'public_member' => 22,
  )),
  0 => 
  \cr::__set_state(array(
     'priv_member' => 3,
     'public_member' => 3,
  )),
  1 => 
  \cr::__set_state(array(
     'priv_member' => 4,
     'public_member' => 4,
  )),
  2 => 
  \cr::__set_state(array(
     'priv_member' => -15,
     'public_member' => -15,
  )),
);
var_dump(array_uintersect_uassoc($a, $b, array("cr", "comp_func_cr"), "comp_func"));
array(2) {
  [1]=>
  object(cr)#%d (2) {
    ["priv_member":"cr":private]=>
    int(4)
    ["public_member"]=>
    int(4)
  }
  [2]=>
  object(cr)#%d (2) {
    ["priv_member":"cr":private]=>
    int(-15)
    ["public_member"]=>
    int(-15)
  }
}
end   ------------ array_uintersect_uassoc() with method --------
