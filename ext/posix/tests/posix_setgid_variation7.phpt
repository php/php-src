--TEST--
Test function posix_setgid() by substituting argument 1 with string values.
--CREDITS--
Marco Fabbri mrfabbri@gmail.com
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--SKIPIF--
<?php
        if(!extension_loaded("posix")) print "skip - POSIX extension not loaded";
?>
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
  var_dump(posix_setgid( $var  ) );
}
?>
--EXPECTF--
*** Test substituting argument 1 with string values ***

Warning: posix_setgid() expects parameter 1 to be long, string given in %s on line %d
bool(false)

Warning: posix_setgid() expects parameter 1 to be long, string given in %s on line %d
bool(false)

Warning: posix_setgid() expects parameter 1 to be long, string given in %s on line %d
bool(false)

Warning: posix_setgid() expects parameter 1 to be long, string given in %s on line %d
bool(false)
