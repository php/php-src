--TEST--
Bug #53588 (SplMinHeap bad sorting with custom compare function)
--FILE--
<?php
class MySimpleHeap extends SplMinHeap{
    public function  compare( $value1, $value2 ){
        return ( $value1 - $value2 );
    }
}

$obj = new MySimpleHeap();
$obj->insert( 8 );
$obj->insert( 0 );
$obj->insert( 4 );

foreach( $obj as $number ) {
    echo $number, "\n";
}
--EXPECT--
0
4
8

