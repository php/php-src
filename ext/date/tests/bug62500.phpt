--TEST--
Bug #62500 (Segfault in DateInterval class when extended)
--FILE--
<?php
class DateCrasher extends DateInterval {
	public function __construct($time_spec) {
		var_dump($this->y);
		$this->y = 3;
		var_dump($this->y);
		parent::__construct($time_spec);
	}
}

$c = new DateCrasher('P2Y4DT6H8M');
var_dump($c);
?>
==DONE==
--EXPECTF--
Warning: DateCrasher::__construct(): The DateInterval object has not been correctly initialized by its constructor in %s on line %d
NULL

Warning: DateCrasher::__construct(): The DateInterval object has not been correctly initialized by its constructor in %s on line %d

Warning: DateCrasher::__construct(): The DateInterval object has not been correctly initialized by its constructor in %s on line %d
NULL
object(DateCrasher)#1 (%d) {
  ["y"]=>
  int(2)
  ["m"]=>
  int(0)
  ["d"]=>
  int(4)
  ["h"]=>
  int(6)
  ["i"]=>
  int(8)
  ["s"]=>
  int(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  bool(false)
}
==DONE==