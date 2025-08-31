--TEST--
ZE2 iterators and exceptions
--FILE--
<?php
class Test implements Iterator
{
    public $arr = array(1, 2, 3);
    public $x = 0;

    public function rewind(): void    { if ($this->x == 0) throw new Exception(__METHOD__); reset($this->arr); }
    public function current(): mixed   { if ($this->x == 1) throw new Exception(__METHOD__); return current($this->arr); }
    public function key(): mixed       { if ($this->x == 2) throw new Exception(__METHOD__); return key($this->arr); }
    public function next(): void      { if ($this->x == 3) throw new Exception(__METHOD__); next($this->arr); }
    public function valid(): bool     { if ($this->x == 4) throw new Exception(__METHOD__); return (key($this->arr) !== NULL); }
}

$t = new Test();

while($t->x < 5)
{
    try
    {
        foreach($t as $k => $v)
        {
            echo "Current\n";
        }
    }
    catch(Exception $e)
    {
        echo "Caught in " . $e->getMessage() . "()\n";
    }
    $t->x++;
}
?>
--EXPECT--
Caught in Test::rewind()
Caught in Test::current()
Caught in Test::key()
Current
Caught in Test::next()
Caught in Test::valid()
