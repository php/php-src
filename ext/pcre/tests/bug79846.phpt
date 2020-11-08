--TEST--
Bug #79846 (8c67c166996 broke simple regexp)
--FILE--
<?php
$item = "component_phase_1";
preg_match("/([a-z]+_[a-z]+_*[a-z]+)_?(\d+)?/", $item, $match);
var_dump($match);
?>
--EXPECT--
array(3) {
  [0]=>
  string(17) "component_phase_1"
  [1]=>
  string(15) "component_phase"
  [2]=>
  string(1) "1"
}
