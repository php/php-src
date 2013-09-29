--TEST--
testing anonymous inheritance
--FILE--
<?php
class Outer {
    protected $data;
    
    public function __construct($data) {
        $this->data = $data;
    }
    
    public function getArrayAccess() {
        /* create a proxy object implementing array access */
        return new class extends Outer implements ArrayAccess {
            public function offsetGet($offset)          { return $this->data[$offset]; }
            public function offsetSet($offset, $data)   { return ($this->data[$offset] = $data); }
            public function offsetUnset($offset)        { unset($this->data[$offset]); }
            public function offsetExists($offset)       { return isset($this->data[$offset]); }
        }($this->data);
    }
}

$outer = new Outer(array(
    rand(1, 100)
));

$proxy = $outer->getArrayAccess();

var_dump($proxy[0]);
?>
--EXPECTF--	
int(%d)


