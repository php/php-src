--TEST--
Test function posix_setuid() by substituting argument 1 with string values.
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


echo "*** Test substituting argument 1 with string values ***\n";



$heredoc = <<<EOT
hello world
EOT;

$variation_array = array(
  'string DQ' => "string",
  'string SQ' => 'string',
  'mixed case string' => "sTrInG",
  'heredoc' => $heredoc,
  );


foreach ( $variation_array as $var ) {
  var_dump(posix_setuid( $var  ) );
}
?>
--EXPECTF--
*** Test substituting argument 1 with string values ***

Warning: posix_setuid() expects parameter 1 to be integer, string given in %s on line 21
bool(false)

Warning: posix_setuid() expects parameter 1 to be integer, string given in %s on line 21
bool(false)

Warning: posix_setuid() expects parameter 1 to be integer, string given in %s on line 21
bool(false)

Warning: posix_setuid() expects parameter 1 to be integer, string given in %s on line 21
bool(false)
