--TEST--
foreach with nested iteratorAggregates
--FILE--
<?php
class EnglishMealIterator implements Iterator {
	private $pos=0;
	private $myContent=array("breakfast", "dinner", "tea");
	
	public function valid() {
		global $indent;
		echo "$indent--> " . __METHOD__ . " ($this->pos)\n";
		return $this->pos<3;
	}
	
	public function next() {
		global $indent;
		echo "$indent--> " . __METHOD__ . " ($this->pos)\n";
		return $this->myContent[$this->pos++];
	}
	
	public function rewind() {
		global $indent;
		echo "$indent--> " . __METHOD__ . " ($this->pos)\n";
		$this->pos=0;
	}

	public function current() {
		global $indent;
		echo "$indent--> " . __METHOD__ . " ($this->pos)\n";
		return $this->myContent[$this->pos];
	}
	
	public function key() {
		global $indent;
		echo "$indent--> " . __METHOD__ . " ($this->pos)\n";
		return "meal " . $this->pos;
	}
	
}

class A1 implements IteratorAggregate {
	function getIterator() {
		return new EnglishMealIterator;
	}
}

class A2 implements IteratorAggregate {
	function getIterator() {
		return new A1;
	}
}

class A3 implements IteratorAggregate {
	function getIterator() {
		return new A2;
	}
}

echo "\n-----( A1: )-----\n";
foreach (new A1 as $k=>$v) {
	echo "$k => $v\n";	
}

echo "\n-----( A2: )-----\n";
foreach (new A2 as $k=>$v) {
	echo "$k => $v\n";	
}

echo "\n-----( A3: )-----\n";
foreach (new A3 as $k=>$v) {
	echo "$k => $v\n";	
}

?>
===DONE===
--EXPECTF--
-----( A1: )-----
--> EnglishMealIterator::rewind (0)
--> EnglishMealIterator::valid (0)
--> EnglishMealIterator::current (0)
--> EnglishMealIterator::key (0)
meal 0 => breakfast
--> EnglishMealIterator::next (0)
--> EnglishMealIterator::valid (1)
--> EnglishMealIterator::current (1)
--> EnglishMealIterator::key (1)
meal 1 => dinner
--> EnglishMealIterator::next (1)
--> EnglishMealIterator::valid (2)
--> EnglishMealIterator::current (2)
--> EnglishMealIterator::key (2)
meal 2 => tea
--> EnglishMealIterator::next (2)
--> EnglishMealIterator::valid (3)

-----( A2: )-----
--> EnglishMealIterator::rewind (0)
--> EnglishMealIterator::valid (0)
--> EnglishMealIterator::current (0)
--> EnglishMealIterator::key (0)
meal 0 => breakfast
--> EnglishMealIterator::next (0)
--> EnglishMealIterator::valid (1)
--> EnglishMealIterator::current (1)
--> EnglishMealIterator::key (1)
meal 1 => dinner
--> EnglishMealIterator::next (1)
--> EnglishMealIterator::valid (2)
--> EnglishMealIterator::current (2)
--> EnglishMealIterator::key (2)
meal 2 => tea
--> EnglishMealIterator::next (2)
--> EnglishMealIterator::valid (3)

-----( A3: )-----
--> EnglishMealIterator::rewind (0)
--> EnglishMealIterator::valid (0)
--> EnglishMealIterator::current (0)
--> EnglishMealIterator::key (0)
meal 0 => breakfast
--> EnglishMealIterator::next (0)
--> EnglishMealIterator::valid (1)
--> EnglishMealIterator::current (1)
--> EnglishMealIterator::key (1)
meal 1 => dinner
--> EnglishMealIterator::next (1)
--> EnglishMealIterator::valid (2)
--> EnglishMealIterator::current (2)
--> EnglishMealIterator::key (2)
meal 2 => tea
--> EnglishMealIterator::next (2)
--> EnglishMealIterator::valid (3)
===DONE===
