--TEST--
proc_open() with > 16 pipes 
--FILE--
<?php

for ($i = 3; $i<= 30; $i++) {
	$spec[$i] = array('pipe', 'w');
}

proc_open('sleep 1', $spec, $pipes);

var_dump(count($spec));
var_dump($pipes);

?>
--EXPECTF--
int(28)
array(28) {
  [3]=>
  resource(4) of type (Unknown)
  [4]=>
  resource(5) of type (Unknown)
  [5]=>
  resource(6) of type (Unknown)
  [6]=>
  resource(7) of type (Unknown)
  [7]=>
  resource(8) of type (Unknown)
  [8]=>
  resource(9) of type (Unknown)
  [9]=>
  resource(10) of type (Unknown)
  [10]=>
  resource(11) of type (Unknown)
  [11]=>
  resource(12) of type (Unknown)
  [12]=>
  resource(13) of type (Unknown)
  [13]=>
  resource(14) of type (Unknown)
  [14]=>
  resource(15) of type (Unknown)
  [15]=>
  resource(16) of type (Unknown)
  [16]=>
  resource(17) of type (Unknown)
  [17]=>
  resource(18) of type (Unknown)
  [18]=>
  resource(19) of type (Unknown)
  [19]=>
  resource(20) of type (Unknown)
  [20]=>
  resource(21) of type (Unknown)
  [21]=>
  resource(22) of type (Unknown)
  [22]=>
  resource(23) of type (Unknown)
  [23]=>
  resource(24) of type (Unknown)
  [24]=>
  resource(25) of type (Unknown)
  [25]=>
  resource(26) of type (Unknown)
  [26]=>
  resource(27) of type (Unknown)
  [27]=>
  resource(28) of type (Unknown)
  [28]=>
  resource(29) of type (Unknown)
  [29]=>
  resource(30) of type (Unknown)
  [30]=>
  resource(31) of type (Unknown)
}
