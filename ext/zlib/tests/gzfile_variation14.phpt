--TEST--
Test function gzfile() by substituting argument 2 with string values.
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
  var_dump(gzfile( $filename, $var  ) );
}
?>
===DONE===
--EXPECTF--
Warning: gzfile() expects parameter 2 to be int, string given in %s on line %d
NULL

Warning: gzfile() expects parameter 2 to be int, string given in %s on line %d
NULL

Warning: gzfile() expects parameter 2 to be int, string given in %s on line %d
NULL

Warning: gzfile() expects parameter 2 to be int, string given in %s on line %d
NULL
===DONE===
