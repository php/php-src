--TEST--
testing reusing anons that implement an interface
--FILE--
<?php
class Outer {
    protected $data;

    public function __construct(&$data) {
        /* array access will be implemented by the time we get to here */
        $this->data = &$data;
    }

    public function getArrayAccess() {
        /* create a child object implementing array access */
        /* this grants you access to protected methods and members */
        return new class($this->data) implements ArrayAccess {
            public function offsetGet($offset) { return $this->data[$offset]; }
            public function offsetSet($offset, $data) { return ($this->data[$offset] = $data); }
            public function offsetUnset($offset) { unset($this->data[$offset]); }
            public function offsetExists($offset) { return isset($this->data[$offset]); }
        };
    }
}

$data = array(
    rand(1, 100),
    rand(2, 200)
);

$outer = new Outer($data);
$proxy = $outer->getArrayAccess();

/* null because no inheritance, so no access to protected member */
var_dump(@$outer->getArrayAccess()[0]);
--EXPECT--
NULL
