--TEST--
Test normal operation of password_get_info() with Yescrypt
--FILE--
<?php

var_dump(password_get_info('$y$jC5//$7NbMKtqBsR3PDV5JHBYPDtaRD3nRg/$FT4mgFH/6EJNHRAGvD6yvzGjCo01KpIhLwGbQW.Nxk1'));
var_dump(password_get_info('$y$jC50..$bBICnZqGiE5P5h4KjoYGpp4S773JB/$ZcD9FJW35.VG0kN0hh5C7oXa3o3dSBSXg/WDaTiWsA8'));
var_dump(password_get_info('$y$jA.0./$pU1KtJbSnMYKcNIQZZLPpAXFpZ4RB/$TeI5bGZQ5l589gWeUjaJzSlIPQZk7Wp2gLsnVG0gJH6'));

var_dump(password_get_info('$y$jA.0./$'));

echo "OK!";

?>
--EXPECT--
array(3) {
  ["algo"]=>
  string(1) "y"
  ["algoName"]=>
  string(8) "yescrypt"
  ["options"]=>
  array(4) {
    ["block_count"]=>
    int(32768)
    ["block_size"]=>
    int(8)
    ["parallelism"]=>
    int(1)
    ["time"]=>
    int(2)
  }
}
array(3) {
  ["algo"]=>
  string(1) "y"
  ["algoName"]=>
  string(8) "yescrypt"
  ["options"]=>
  array(4) {
    ["block_count"]=>
    int(32768)
    ["block_size"]=>
    int(8)
    ["parallelism"]=>
    int(2)
    ["time"]=>
    int(1)
  }
}
array(3) {
  ["algo"]=>
  string(1) "y"
  ["algoName"]=>
  string(8) "yescrypt"
  ["options"]=>
  array(4) {
    ["block_count"]=>
    int(8192)
    ["block_size"]=>
    int(1)
    ["parallelism"]=>
    int(2)
    ["time"]=>
    int(2)
  }
}
array(3) {
  ["algo"]=>
  NULL
  ["algoName"]=>
  string(7) "unknown"
  ["options"]=>
  array(0) {
  }
}
OK!
