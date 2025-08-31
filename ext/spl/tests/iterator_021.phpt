--TEST--
SPL: RecursiveIteratorIterator and hasChildren
--FILE--
<?php

class MyRecursiveArrayIterator extends RecursiveArrayIterator
{
    function valid(): bool
    {
        if (!parent::valid())
        {
            echo __METHOD__ . " = false\n";
            return false;
        }
        else
        {
            return true;
        }
    }

    function getChildren(): ?RecursiveArrayIterator
    {
        echo __METHOD__ . "\n";
        return parent::getChildren();
    }
}

class RecursiveArrayIteratorIterator extends RecursiveIteratorIterator
{
    private $max_depth;
    private $over = 0;
    private $skip = false;

    function __construct($it, $max_depth)
    {
        $this->max_depth = $max_depth;
        parent::__construct($it);
    }

    function rewind(): void
    {
        echo __METHOD__ . "\n";
        $this->skip = false;
        parent::rewind();
    }

    function valid(): bool
    {
        echo __METHOD__ . "\n";
        if ($this->skip)
        {
            $this->skip = false;
            $this->next();
        }
        return parent::valid();
    }

    function current(): mixed
    {
        echo __METHOD__ . "\n";
        return parent::current();
    }

    function key(): mixed
    {
        echo __METHOD__ . "\n";
        return parent::key();
    }

    function next(): void
    {
        echo __METHOD__ . "\n";
        parent::next();
    }

    function callHasChildren(): bool
    {
        $this->skip = false;
        $has = parent::callHasChildren();
        $res = $this->getDepth() < $this->max_depth && $has;
        echo __METHOD__ . "(".$this->getDepth().") = ".($res?"yes":"no")."/".($has?"yes":"no")."\n";
        if ($has && !$res)
        {
            $this->over++;
            if ($this->over == 2) {
                $this->skip = true;
            }
        }
        return $res;
    }

    function beginChildren(): void
    {
        echo __METHOD__ . "(".$this->getDepth().")\n";
    }

    function endChildren(): void
    {
        echo __METHOD__ . "(".$this->getDepth().")\n";
    }
}

foreach(new RecursiveArrayIteratorIterator(new MyRecursiveArrayIterator(array("a", array("ba", array("bba", "bbb"), array(array("bcaa"), array("bcba"))), array("ca"), "d")), 2) as $k=>$v)
{
    if (is_array($v)) $v = join('',$v);
    echo "$k=>$v\n";
}
?>
--EXPECT--
RecursiveArrayIteratorIterator::rewind
RecursiveArrayIteratorIterator::callHasChildren(0) = no/no
RecursiveArrayIteratorIterator::valid
RecursiveArrayIteratorIterator::current
RecursiveArrayIteratorIterator::key
0=>a
RecursiveArrayIteratorIterator::next
RecursiveArrayIteratorIterator::callHasChildren(0) = yes/yes
MyRecursiveArrayIterator::getChildren
RecursiveArrayIteratorIterator::beginChildren(1)
RecursiveArrayIteratorIterator::callHasChildren(1) = no/no
RecursiveArrayIteratorIterator::valid
RecursiveArrayIteratorIterator::current
RecursiveArrayIteratorIterator::key
0=>ba
RecursiveArrayIteratorIterator::next
RecursiveArrayIteratorIterator::callHasChildren(1) = yes/yes
MyRecursiveArrayIterator::getChildren
RecursiveArrayIteratorIterator::beginChildren(2)
RecursiveArrayIteratorIterator::callHasChildren(2) = no/no
RecursiveArrayIteratorIterator::valid
RecursiveArrayIteratorIterator::current
RecursiveArrayIteratorIterator::key
0=>bba
RecursiveArrayIteratorIterator::next
RecursiveArrayIteratorIterator::callHasChildren(2) = no/no
RecursiveArrayIteratorIterator::valid
RecursiveArrayIteratorIterator::current
RecursiveArrayIteratorIterator::key
1=>bbb
RecursiveArrayIteratorIterator::next
MyRecursiveArrayIterator::valid = false
RecursiveArrayIteratorIterator::endChildren(2)
RecursiveArrayIteratorIterator::callHasChildren(1) = yes/yes
MyRecursiveArrayIterator::getChildren
RecursiveArrayIteratorIterator::beginChildren(2)
RecursiveArrayIteratorIterator::callHasChildren(2) = no/yes
RecursiveArrayIteratorIterator::valid
RecursiveArrayIteratorIterator::current
RecursiveArrayIteratorIterator::key
0=>bcaa
RecursiveArrayIteratorIterator::next
RecursiveArrayIteratorIterator::callHasChildren(2) = no/yes
RecursiveArrayIteratorIterator::valid
RecursiveArrayIteratorIterator::next
MyRecursiveArrayIterator::valid = false
RecursiveArrayIteratorIterator::endChildren(2)
MyRecursiveArrayIterator::valid = false
RecursiveArrayIteratorIterator::endChildren(1)
RecursiveArrayIteratorIterator::callHasChildren(0) = yes/yes
MyRecursiveArrayIterator::getChildren
RecursiveArrayIteratorIterator::beginChildren(1)
RecursiveArrayIteratorIterator::callHasChildren(1) = no/no
RecursiveArrayIteratorIterator::current
RecursiveArrayIteratorIterator::key
0=>ca
RecursiveArrayIteratorIterator::next
MyRecursiveArrayIterator::valid = false
RecursiveArrayIteratorIterator::endChildren(1)
RecursiveArrayIteratorIterator::callHasChildren(0) = no/no
RecursiveArrayIteratorIterator::valid
RecursiveArrayIteratorIterator::current
RecursiveArrayIteratorIterator::key
3=>d
RecursiveArrayIteratorIterator::next
MyRecursiveArrayIterator::valid = false
RecursiveArrayIteratorIterator::valid
MyRecursiveArrayIterator::valid = false
