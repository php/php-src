--TEST--
Test Uri\QueryParams::append() - success - strings representing non-finite floats
--FILE--
<?php

$params = (new Uri\QueryParams())->append("positive", "INF")->append("negative", "-INF")->append("nan", "NAN");

var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());

?>
--EXPECT--
array(3) {
  [0]=>
  array(2) {
    [0]=>
    string(8) "positive"
    [1]=>
    string(3) "INF"
  }
  [1]=>
  array(2) {
    [0]=>
    string(8) "negative"
    [1]=>
    string(4) "-INF"
  }
  [2]=>
  array(2) {
    [0]=>
    string(3) "nan"
    [1]=>
    string(3) "NAN"
  }
}
string(34) "positive=INF&negative=-INF&nan=NAN"
string(34) "positive=INF&negative=-INF&nan=NAN"
string(34) "positive=INF&negative=-INF&nan=NAN"
