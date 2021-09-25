--TEST--
foreach with iteratorAggregate
--FILE--
<?php
class EnglishMealIterator implements Iterator {
    private $pos=0;
    private $myContent=array("breakfast", "dinner", "tea");

    public function valid(): bool {
        global $indent;
        echo "$indent--> " . __METHOD__ . " ($this->pos)\n";
        return $this->pos < count($this->myContent);
    }

    public function next(): void {
        global $indent;
        echo "$indent--> " . __METHOD__ . " ($this->pos)\n";
        $this->pos++;
    }

    public function rewind(): void {
        global $indent;
        echo "$indent--> " . __METHOD__ . " ($this->pos)\n";
        $this->pos=0;
    }

    public function current(): mixed {
        global $indent;
        echo "$indent--> " . __METHOD__ . " ($this->pos)\n";
        return $this->myContent[$this->pos];
    }

    public function key(): mixed {
        global $indent;
        echo "$indent--> " . __METHOD__ . " ($this->pos)\n";
        return "meal " . $this->pos;
    }

}

class FrenchMealIterator implements Iterator {
    private $pos=0;
    private $myContent=array("petit dejeuner", "dejeuner", "gouter", "dinner");

    public function valid(): bool {
        global $indent;
        echo "$indent--> " . __METHOD__ . " ($this->pos)\n";
        return $this->pos < count($this->myContent);
    }

    public function next(): void {
        global $indent;
        echo "$indent--> " . __METHOD__ . " ($this->pos)\n";
        $this->pos++;
    }

    public function rewind(): void {
        global $indent;
        echo "$indent--> " . __METHOD__ . " ($this->pos)\n";
        $this->pos=0;
    }

    public function current(): mixed {
        global $indent;
        echo "$indent--> " . __METHOD__ . " ($this->pos)\n";
        return $this->myContent[$this->pos];
    }

    public function key(): mixed {
        global $indent;
        echo "$indent--> " . __METHOD__ . " ($this->pos)\n";
        return "meal " . $this->pos;
    }

}


Class EuropeanMeals implements IteratorAggregate {

    private $storedEnglishMealIterator;
    private $storedFrenchMealIterator;

    public function __construct() {
        $this->storedEnglishMealIterator = new EnglishMealIterator;
        $this->storedFrenchMealIterator = new FrenchMealIterator;
    }

    public function getIterator(): Traversable {
        global $indent;
        echo "$indent--> " . __METHOD__  . "\n";

        //Alternate between English and French meals
        static $i = 0;
        if ($i++%2 == 0) {
            return $this->storedEnglishMealIterator;
        } else {
            return $this->storedFrenchMealIterator;
        }
    }

}

$f = new EuropeanMeals;
var_dump($f);

echo "-----( Simple iteration 1: )-----\n";
foreach ($f as $k=>$v) {
    echo "$k => $v\n";
}
echo "-----( Simple iteration 2: )-----\n";
foreach ($f as $k=>$v) {
    echo "$k => $v\n";
}


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
--EXPECTF--
object(EuropeanMeals)#%d (2) {
  ["storedEnglishMealIterator":"EuropeanMeals":private]=>
  object(EnglishMealIterator)#%d (2) {
    ["pos":"EnglishMealIterator":private]=>
    int(0)
    ["myContent":"EnglishMealIterator":private]=>
    array(3) {
      [0]=>
      string(9) "breakfast"
      [1]=>
      string(6) "dinner"
      [2]=>
      string(3) "tea"
    }
  }
  ["storedFrenchMealIterator":"EuropeanMeals":private]=>
  object(FrenchMealIterator)#%d (2) {
    ["pos":"FrenchMealIterator":private]=>
    int(0)
    ["myContent":"FrenchMealIterator":private]=>
    array(4) {
      [0]=>
      string(14) "petit dejeuner"
      [1]=>
      string(8) "dejeuner"
      [2]=>
      string(6) "gouter"
      [3]=>
      string(6) "dinner"
    }
  }
}
-----( Simple iteration 1: )-----
--> EuropeanMeals::getIterator
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
-----( Simple iteration 2: )-----
--> EuropeanMeals::getIterator
--> FrenchMealIterator::rewind (0)
--> FrenchMealIterator::valid (0)
--> FrenchMealIterator::current (0)
--> FrenchMealIterator::key (0)
meal 0 => petit dejeuner
--> FrenchMealIterator::next (0)
--> FrenchMealIterator::valid (1)
--> FrenchMealIterator::current (1)
--> FrenchMealIterator::key (1)
meal 1 => dejeuner
--> FrenchMealIterator::next (1)
--> FrenchMealIterator::valid (2)
--> FrenchMealIterator::current (2)
--> FrenchMealIterator::key (2)
meal 2 => gouter
--> FrenchMealIterator::next (2)
--> FrenchMealIterator::valid (3)
--> FrenchMealIterator::current (3)
--> FrenchMealIterator::key (3)
meal 3 => dinner
--> FrenchMealIterator::next (3)
--> FrenchMealIterator::valid (4)



-----( Nested iteration: )-----
 --> EuropeanMeals::getIterator
 --> EnglishMealIterator::rewind (3)
 --> EnglishMealIterator::valid (0)
 --> EnglishMealIterator::current (0)
 --> EnglishMealIterator::key (0)

Top level 1: 
meal 0 => breakfast
     --> EuropeanMeals::getIterator
     --> FrenchMealIterator::rewind (4)
     --> FrenchMealIterator::valid (0)
     --> FrenchMealIterator::current (0)
     --> FrenchMealIterator::key (0)
     meal 0 => petit dejeuner
     --> FrenchMealIterator::next (0)
     --> FrenchMealIterator::valid (1)
     --> FrenchMealIterator::current (1)
     --> FrenchMealIterator::key (1)
     meal 1 => dejeuner
     --> FrenchMealIterator::next (1)
     --> FrenchMealIterator::valid (2)
     --> FrenchMealIterator::current (2)
     --> FrenchMealIterator::key (2)
     meal 2 => gouter
     --> FrenchMealIterator::next (2)
     --> FrenchMealIterator::valid (3)
     --> FrenchMealIterator::current (3)
     --> FrenchMealIterator::key (3)
     meal 3 => dinner
     --> FrenchMealIterator::next (3)
     --> FrenchMealIterator::valid (4)
 --> EnglishMealIterator::next (0)
 --> EnglishMealIterator::valid (1)
 --> EnglishMealIterator::current (1)
 --> EnglishMealIterator::key (1)

Top level 2: 
meal 1 => dinner
     --> EuropeanMeals::getIterator
     --> EnglishMealIterator::rewind (1)
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
 --> EnglishMealIterator::next (3)
 --> EnglishMealIterator::valid (4)
