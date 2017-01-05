--TEST--
Bug #38624 (Strange warning when incrementing an object property and exception is thrown from __get method)
--FILE--
<?php

class impl
{
    public function __construct()
    {
       $this->counter++;
    }
    public function __set( $name, $value )
    {
        throw new Exception( "doesn't work" );
    }

    public function __get( $name )
    {
        throw new Exception( "doesn't work" );
    }

}

$impl = new impl();

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Uncaught Exception: doesn't work in %s:%d
Stack trace:
#0 %s(%d): impl->__get('counter')
#1 %s(%d): impl->__construct()
#2 {main}
  thrown in %s on line %d
