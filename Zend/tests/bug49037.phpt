--TEST--
Bug #49037 (@list( $b ) = $a; causes a crash)
--FILE--
<?php
$a = array( "c" );
@list( $b ) = $a;
print_r( $a );
?>
--EXPECT--
Array
(
    [0] => c
)

