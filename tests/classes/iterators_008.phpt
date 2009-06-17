--TEST--
Ensure plain userspace superclass does not override special iterator behaviour on child class.
--FILE--
<?php
Class C {}

class D extends C implements Iterator {
  
  private $counter = 2;
  
  public function valid() {
    echo __METHOD__ . "($this->counter)\n";
    return $this->counter;    
  }
  
  public function next() {
    $this->counter--;   
    echo __METHOD__ . "($this->counter)\n";
  }
  
  public function rewind() {
    echo __METHOD__ . "($this->counter)\n";
  }
  
  public function current() {
    echo __METHOD__ . "($this->counter)\n";
  }
  
  public function key() {
    echo __METHOD__ . "($this->counter)\n";
  }
  
}

foreach (new D as $x) {}
?>
--EXPECTF--
D::rewind(2)
D::valid(2)
D::current(2)
D::next(1)
D::valid(1)
D::current(1)
D::next(0)
D::valid(0)