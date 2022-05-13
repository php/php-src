--TEST--
SPL: RecursiveIteratorIterator and beginChildren/endChildren
--FILE--
<?php

class Menu extends ArrayObject
{
    function getIterator(): RecursiveArrayIterator
    {
        echo __METHOD__ . "\n";
        return new RecursiveArrayIterator($this);
    }
}

class MenuOutput extends RecursiveIteratorIterator
{
    function __construct(Menu $it)
    {
        parent::__construct($it);
    }
    function rewind(): void
    {
        echo "<ul>\n";
        parent::rewind();
    }
    function beginChildren(): void
    {
        echo str_repeat('  ',$this->getDepth())."<ul>\n";
    }

    function endChildren(): void
    {
        echo str_repeat('  ',$this->getDepth())."</ul>\n";
    }
    function valid(): bool
    {
        if (!parent::valid()) {
            echo "<ul>\n";
            return false;
        }
        return true;
    }
}

$arr = array("a", array("ba", array("bba", "bbb"), array(array("bcaa"))), array("ca"), "d");
$obj = new Menu($arr);
$rit = new MenuOutput($obj);
foreach($rit as $k=>$v)
{
    echo str_repeat('  ',$rit->getDepth()+1)."$k=>$v\n";
}
?>
--EXPECT--
Menu::getIterator
<ul>
  0=>a
  <ul>
    0=>ba
    <ul>
      0=>bba
      1=>bbb
    </ul>
    <ul>
      <ul>
        0=>bcaa
      </ul>
    </ul>
  </ul>
  <ul>
    0=>ca
  </ul>
  3=>d
<ul>
