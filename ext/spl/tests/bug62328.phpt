--TEST--
Bug #62328 (cast_object takes precedence over __toString)
--CREDITS--
leight at gmail dot com
--FILE--
<?php

class SplFileInfo62328 extends SplFileInfo
{
    public function __toString()
    {
        return '__toString';
    }
}

$fi = new SplFileInfo62328(__FILE__);

echo (string)$fi . PHP_EOL;
echo (string)$fi->__toString() . PHP_EOL;

?>
--EXPECT--
__toString
__toString
