--TEST--
Test function readgzfile() by substituting argument 2 with string values.
--SKIPIF--
<?php
if (!extension_loaded('zlib')) die ('skip zlib extension not available in this build');
?>
--FILE--
<?php


$filename = $filename = dirname(__FILE__)."/004.txt.gz";


$heredoc = <<<EOT
hello world
EOT;

$variation_array = array(
  'string DQ' => "string",
  'string SQ' => 'string',
  'mixed case string' => "sTrInG",
  'heredoc' => $heredoc
  );


foreach ( $variation_array as $var ) {
  var_dump(readgzfile( $filename, $var  ) );
}
?>
===DONE===
--EXPECTF--
Warning: readgzfile() expects parameter 2 to be integer, string given in %s on line %d
NULL

Warning: readgzfile() expects parameter 2 to be integer, string given in %s on line %d
NULL

Warning: readgzfile() expects parameter 2 to be integer, string given in %s on line %d
NULL

Warning: readgzfile() expects parameter 2 to be integer, string given in %s on line %d
NULL
===DONE===
