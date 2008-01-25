--TEST--
array_udiff():Test return type and value for expected input 
--FILE--
<?php
/*
* proto array array_udiff ( array $array1, array $array2 [, array $ ..., callback $data_compare_func] )
* Function is implemented in ext/standard/array.c
*/
class cr {
    private $priv_member;
    function cr($val) {
        $this->priv_member = $val;
    }
    static function comp_func_cr($a, $b) {
        if ($a->priv_member === $b->priv_member) return 0;
        return ($a->priv_member > $b->priv_member) ? 1 : -1;
    }
}
$a = array("0.1" => new cr(9), "0.5" => new cr(12), 0 => new cr(23), 1 => new cr(4), 2 => new cr(-15),);
$b = array("0.2" => new cr(9), "0.5" => new cr(22), 0 => new cr(3), 1 => new cr(4), 2 => new cr(-15),);
$result = array_udiff($a, $b, array("cr", "comp_func_cr"));
var_dump($result);
?>
--EXPECTF--
array(2) {
  ["0.5"]=>
  object(cr)#%d (1) {
    ["priv_member":"cr":private]=>
    int(12)
  }
  [0]=>
  object(cr)#%d (1) {
    ["priv_member":"cr":private]=>
    int(23)
  }
}