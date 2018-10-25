--TEST--
Bug #34467 (foreach + __get + __set incosistency)
--FILE--
<?php
class abc {
	private $arr;

	function __set ($key, $value) {
    $this->arr[$key] = $value;
  }

	function __get ($key) {
	  return $this->arr[$key];
	}
}
$abc = new abc();
foreach (array (1,2,3) as $abc->k => $abc->v) {
	var_dump($abc->k,$abc->v);
}
?>
--EXPECT--
int(0)
int(1)
int(1)
int(2)
int(2)
int(3)
