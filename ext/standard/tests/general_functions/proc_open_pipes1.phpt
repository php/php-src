--TEST--
proc_open() with > 16 pipes
--FILE--
<?php

for ($i = 3; $i<= 30; $i++) {
    $spec[$i] = array('pipe', 'w');
}

$php = getenv("TEST_PHP_EXECUTABLE_ESCAPED");
$callee = escapeshellarg(__DIR__ . "/proc_open_pipes_sleep.inc");
proc_open("$php -n $callee", $spec, $pipes);

var_dump(count($spec));
var_dump($pipes);

?>
--EXPECTF--
int(28)
array(28) {
  [3]=>
  resource(%d) of type (Unknown)
  [4]=>
  resource(%d) of type (Unknown)
  [5]=>
  resource(%d) of type (Unknown)
  [6]=>
  resource(%d) of type (Unknown)
  [7]=>
  resource(%d) of type (Unknown)
  [8]=>
  resource(%d) of type (Unknown)
  [9]=>
  resource(%d) of type (Unknown)
  [10]=>
  resource(%d) of type (Unknown)
  [11]=>
  resource(%d) of type (Unknown)
  [12]=>
  resource(%d) of type (Unknown)
  [13]=>
  resource(%d) of type (Unknown)
  [14]=>
  resource(%d) of type (Unknown)
  [15]=>
  resource(%d) of type (Unknown)
  [16]=>
  resource(%d) of type (Unknown)
  [17]=>
  resource(%d) of type (Unknown)
  [18]=>
  resource(%d) of type (Unknown)
  [19]=>
  resource(%d) of type (Unknown)
  [20]=>
  resource(%d) of type (Unknown)
  [21]=>
  resource(%d) of type (Unknown)
  [22]=>
  resource(%d) of type (Unknown)
  [23]=>
  resource(%d) of type (Unknown)
  [24]=>
  resource(%d) of type (Unknown)
  [25]=>
  resource(%d) of type (Unknown)
  [26]=>
  resource(%d) of type (Unknown)
  [27]=>
  resource(%d) of type (Unknown)
  [28]=>
  resource(%d) of type (Unknown)
  [29]=>
  resource(%d) of type (Unknown)
  [30]=>
  resource(%d) of type (Unknown)
}
