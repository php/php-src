--TEST--
Bug #32993 (implemented Iterator function current() don't throw exception)
--FILE--
<?php
class Test implements Iterator {

    public $arr = array();

    public function rewind()    { return reset($this->arr); }
    public function current()   { throw new Exception(); }
    public function key()       { return key($this->arr); }
    public function next()      { return next($this->arr); }
    public function valid()     { return (current($this->arr) !== false); }
}

$t = new Test();
$t->arr =  array(1, 2, 3);

try {
    foreach ($t as $v) {
        echo "$v\n";
    }
} catch (Exception $e) {
    ; // handle exception
}
echo "ok\n";
?>
--EXPECT--
ok
