--TEST--
Test typed properties basic operation
--FILE--
<?php
var_dump(new class(1, 2.2, true, ["four"], new stdClass) {
	public int $int;
	public float $float;
	public bool $bool;
	public array $array;
	public stdClass $std;
	public iterable $it;

	public function __construct(int $int, float $float, bool $bool, array $array, stdClass $std) {
		$this->int = $int;
		$this->float = $float;
		$this->bool = $bool;
		$this->array = $array;
		$this->std = $std;
		$this->it = $array;
	}
});
?>
--EXPECTF--
object(class@anonymous)#%d (6) {
  ["int"]=>
  int(1)
  ["float"]=>
  float(2.2)
  ["bool"]=>
  bool(true)
  ["array"]=>
  array(1) {
    [0]=>
    string(4) "four"
  }
  ["std"]=>
  object(stdClass)#%d (0) {
  }
  ["it"]=>
  array(1) {
    [0]=>
    string(4) "four"
  }
}
