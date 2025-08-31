--TEST--
GH-7958 (Nested CallbackFilterIterator is leaking memory)
--FILE--
<?php
class Action
{
    public \Iterator $iterator;

    public function __construct(array $data)
    {
        $this->iterator = new ArrayIterator($data);
        echo '-- c ' . spl_object_id($this) . "\n";
    }

    public function __destruct()
    {
        echo '-- d ' . spl_object_id($this) . "\n";
    }

    public function filter()
    {
        $this->iterator = new \CallbackFilterIterator($this->iterator, fn() => true);
        $this->iterator->rewind();
    }
}

$action = new Action(['a', 'b']);
$action->filter();
$action->filter();
print_r(iterator_to_array($action->iterator));
$action = null;
gc_collect_cycles();
echo "==DONE==\n";
?>
--EXPECT--
-- c 1
Array
(
    [0] => a
    [1] => b
)
-- d 1
==DONE==
