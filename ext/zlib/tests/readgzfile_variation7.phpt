--TEST--
Test function readgzfile() by substituting argument 1 with string values.
--SKIPIF--
<?php
if (!extension_loaded('zlib')) die ('skip zlib extension not available in this build');
?>
--FILE--
<?php


$use_include_path = false;


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
  var_dump(readgzfile( $var ,  $use_include_path ) );
}
?>
===DONE===
--EXPECTF--
Warning: readgzfile(): failed to open stream (string): No such file or directory in %s on line %d
bool(false)

Warning: readgzfile(): failed to open stream (string): No such file or directory in %s on line %d
bool(false)

Warning: readgzfile(): failed to open stream (sTrInG): No such file or directory in %s on line %d
bool(false)

Warning: readgzfile(): failed to open stream (hello world): No such file or directory in %s on line %d
bool(false)
===DONE===
