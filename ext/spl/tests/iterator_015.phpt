--TEST--
SPL: RecursiveIteratorIterator and beginChildren/endChildren
--FILE--
<?php

class RecursiveArrayIteratorIterator extends RecursiveIteratorIterator
{
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
$obj = new RecursiveArrayIterator($arr);
$rit = new RecursiveArrayIteratorIterator($obj);
foreach($rit as $k=>$v)
{
    echo str_repeat('  ',$rit->getDepth()+1)."$k=>$v\n";
}
?>
--EXPECT--
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
