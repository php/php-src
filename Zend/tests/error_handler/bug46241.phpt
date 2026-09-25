--TEST--
Bug #46241 (error handler stacks)
--FILE--
<?php

class ErrorHandling
{

    public function errorHandler1( $errno, $errstr )
    {
        echo "Caught on first level: '$errstr'\n";
        return true;
    }

    public function errorHandler2( $errno, $errstr )
    {
        echo "Caught on second level: '$errstr'\n";
        return true;
    }
}

$err = new ErrorHandling();

set_error_handler( array( $err, 'errorHandler1' ) );
set_error_handler( array( $err, 'errorHandler2' ) );

trigger_error( 'Foo', E_USER_WARNING );

function errorHandler1( $errno, $errstr )
{
    echo "Caught on first level: '$errstr'\n";
    return true;
}

function errorHandler2( $errno, $errstr )
{
    echo "Caught on second level: '$errstr'\n";
    return true;
}

set_error_handler( 'errorHandler1' );
set_error_handler( 'errorHandler2' );

trigger_error( 'Foo', E_USER_WARNING );
?>
==END==
--EXPECT--
Caught on second level: 'Foo'
Caught on second level: 'Foo'
==END==
