--TEST--
Bug #32226 (SEGV with exception handler on non existing instance)
--FILE--
<?php

class A
{
        public function __construct()
        {
                set_exception_handler(array($this, 'EH'));

                throw new Exception();
        }

        public function EH()
        {
                restore_exception_handler();

                throw new Exception();
        }
}

try
{
$a = new A();
}
catch(Exception $e)
{
    echo "Caught\n";
}

?>
--EXPECT--
Caught
