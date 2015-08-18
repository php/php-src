--TEST--
Test function posix_seteuid() by substituting argument 1 with array values.
--SKIPIF--
<?php 
        if(!extension_loaded("posix")) print "skip - POSIX extension not loaded"; 
?>
--CREDITS--
Marco Fabbri mrfabbri@gmail.com
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--FILE--
<?php

$index_array = array(1, 2, 3);
$assoc_array = array(1 => 'one', 2 => 'two');

$variation_array = array(
  'empty array' => array(),
  'int indexed array' => $index_array,
  'associative array' => $assoc_array,
  'nested arrays' => array('foo', $index_array, $assoc_array),
  );


foreach ( $variation_array as $var ) {
  var_dump(posix_seteuid( $var  ) );
}

?>
--EXPECTF--
Warning: posix_seteuid() expects parameter 1 to be integer, array given in %s on line 15
bool(false)

Warning: posix_seteuid() expects parameter 1 to be integer, array given in %s on line 15
bool(false)

Warning: posix_seteuid() expects parameter 1 to be integer, array given in %s on line 15
bool(false)

Warning: posix_seteuid() expects parameter 1 to be integer, array given in %s on line 15
bool(false)
