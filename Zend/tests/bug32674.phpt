--TEST--
Bug #32674 (exception in iterator causes crash)
--FILE--
<?php
class collection implements Iterator {

  private $_elements = array();

  public function __construct() {
  }

  public function rewind() {
    reset($this->_elements);
  }

  public function count() {
    return count($this->_elements);
  }

  public function current() {
    $element = current($this->_elements);
    return $element;
  }

  public function next() {
    $element = next($this->_elements);
    return $element;
  }

  public function key() {
    $this->_fillCollection();
    $element = key($this->_elements);
    return $element;
  }

  public function valid() {
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
