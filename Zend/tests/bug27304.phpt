--TEST--
Bug #27304 (Static functions don't function properly)
--FILE--
<?php

class Staticexample
{
    static function test()
    {
        var_dump(isset($this));
    }
}

$b = new Staticexample();
Staticexample::test();
$b->test();

?>
--EXPECT--
bool(false)
bool(false)
