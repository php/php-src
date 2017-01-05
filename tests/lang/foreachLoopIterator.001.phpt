--TEST--
foreach with Iterator. 
--FILE--
<?php

class MealIterator implements Iterator {
	private $pos=0;
	private $myContent=array("breakfast", "lunch", "dinner");
	
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

$f = new MealIterator;
var_dump($f);

echo "-----( Simple iteration: )-----\n";
foreach ($f as $k=>$v) {
	echo "$k => $v\n";	
}

$f->rewind();

$indent = " ";

echo "\n\n\n-----( Nested iteration: )-----\n";
$count=1;
foreach ($f as $k=>$v) {
	echo "\nTop level "  .  $count++ . ": \n"; 
	echo "$k => $v\n";
	$indent = "     ";
	foreach ($f as $k=>$v) {
		echo "     $k => $v\n";	
	}
	$indent = " ";
	
}

?>
===DONE===
--EXPECTF--
object(MealIterator)#%d (2) {
  ["pos":"MealIterator":private]=>
  int(0)
  ["myContent":"MealIterator":private]=>
  array(3) {
    [0]=>
    string(9) "breakfast"
    [1]=>
    string(5) "lunch"
    [2]=>
    string(6) "dinner"
  }
}
-----( Simple iteration: )-----
--> MealIterator::rewind (0)
--> MealIterator::valid (0)
--> MealIterator::current (0)
--> MealIterator::key (0)
meal 0 => breakfast
--> MealIterator::next (0)
--> MealIterator::valid (1)
--> MealIterator::current (1)
--> MealIterator::key (1)
meal 1 => lunch
--> MealIterator::next (1)
--> MealIterator::valid (2)
--> MealIterator::current (2)
--> MealIterator::key (2)
meal 2 => dinner
--> MealIterator::next (2)
--> MealIterator::valid (3)
--> MealIterator::rewind (3)



-----( Nested iteration: )-----
 --> MealIterator::rewind (0)
 --> MealIterator::valid (0)
 --> MealIterator::current (0)
 --> MealIterator::key (0)

Top level 1: 
meal 0 => breakfast
     --> MealIterator::rewind (0)
     --> MealIterator::valid (0)
     --> MealIterator::current (0)
     --> MealIterator::key (0)
     meal 0 => breakfast
     --> MealIterator::next (0)
     --> MealIterator::valid (1)
     --> MealIterator::current (1)
     --> MealIterator::key (1)
     meal 1 => lunch
     --> MealIterator::next (1)
     --> MealIterator::valid (2)
     --> MealIterator::current (2)
     --> MealIterator::key (2)
     meal 2 => dinner
     --> MealIterator::next (2)
     --> MealIterator::valid (3)
 --> MealIterator::next (3)

Notice: Undefined offset: 3 in %s on line %d
 --> MealIterator::valid (4)
===DONE===
