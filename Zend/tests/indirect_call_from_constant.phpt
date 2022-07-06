--TEST--
Indirect call with constants.
--FILE--
<?php

class Test
{
    public static function method()
    {
        echo "Method called!\n";
    }
}

['Test', 'method']();

'Test::method'();

(['Test', 'method'])();

('Test::method')();

?>
--EXPECT--
Method called!
Method called!
Method called!
Method called!
