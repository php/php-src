--TEST--
Bug GH-7958: Circular reference with LimitIterator is not garbage collected
--FILE--
<?php

class Action
{
    /** @var \Iterator */
    public $generator;

    public function __construct(array $data)
    {
        $this->generator = new class($this, $data) extends \ArrayIterator {
            /** @var Action */
            public $action;

            public function __construct(Action $action, array $data)
            {
                $this->action = $action; // circular ref

                parent::__construct($data);
            }
        };

        echo '-- c ' . spl_object_id($this) . "\n";
    }

    public function __destruct()
    {
        echo '-- d ' . spl_object_id($this) . "\n";
    }

    /**
     * @return $this
     */
    public function filter()
    {
        $filterFx = function ($row) {
            return $row !== $this; // always true, dummy use $this
        };
        $this->generator = new \CallbackFilterIterator($this->generator, $filterFx);
        $this->generator->rewind();

        return $this;
    }

    /**
     * @return $this
     */
    public function limit(int $limit = null, int $offset = 0)
    {
        $this->generator = new \LimitIterator($this->generator, $offset, $limit ?? -1);
        $this->generator->rewind();

        return $this;
    }
}

$action = new Action(['a', 'b']);
print_r(iterator_to_array($action->generator));
$action = null; gc_collect_cycles();

$action = new Action(['a', 'b']);
$action->filter();
print_r(iterator_to_array($action->generator));
$action = null; gc_collect_cycles();

$action = new Action(['a', 'b']);
$action->filter()->limit(1);
print_r(iterator_to_array($action->generator));
$action = null; gc_collect_cycles();

echo '----------------' . "\n";

?>
===DONE===
--EXPECT--
-- c 1
Array
(
    [0] => a
    [1] => b
)
-- d 1
-- c 1
Array
(
    [0] => a
    [1] => b
)
-- d 1
-- c 1
Array
(
    [0] => a
)
-- d 1
----------------
===DONE===
