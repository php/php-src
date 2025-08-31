--TEST--
Bug #32674 (exception in iterator causes crash)
--FILE--
<?php
class collection implements Iterator {

  private $_elements = array();

  public function __construct() {
  }

  public function rewind(): void {
    reset($this->_elements);
  }

  public function count(): int {
    return count($this->_elements);
  }

  public function current(): mixed {
    $element = current($this->_elements);
    return $element;
  }

  public function next(): void {
    $element = next($this->_elements);
    $element;
  }

  public function key(): mixed {
    $this->_fillCollection();
    $element = key($this->_elements);
    return $element;
  }

  public function valid(): bool {
    throw new Exception('shit happened');

    return ($this->current() !== false);
  }
}

class class2 {
  public $dummy;
}

$obj = new class2();
$col = new collection();

try {
    foreach($col as $co) {
    //irrelevant
    }
    echo 'shouldn`t get here';
    //$dummy = 'this will not crash';
    $obj->dummy = 'this will crash';
} catch (Exception $e) {
    echo "ok\n";
}
?>
--EXPECT--
ok
