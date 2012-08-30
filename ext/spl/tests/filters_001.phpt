--TEST--
SPL: Filter
--FILE--
<?php
class TrimFilter implements Filter
{
    public function filter($str)
    {
        return trim($str);
    }
}

$o = new TrimFilter();

var_dump($o instanceof Filter) . PHP_EOL;
var_dump($o->filter(' good '));
?>
--EXPECT--
bool(true)
string(4) "good"