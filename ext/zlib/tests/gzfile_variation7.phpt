--TEST--
Test function gzfile() by substituting argument 1 with string values.
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
  var_dump(gzfile( $var ,  $use_include_path ) );
}
?>
--EXPECTF--
Warning: gzfile(string): Failed to open stream: No such file or directory in %s on line %d
bool(false)

Warning: gzfile(string): Failed to open stream: No such file or directory in %s on line %d
bool(false)

Warning: gzfile(sTrInG): Failed to open stream: No such file or directory in %s on line %d
bool(false)

Warning: gzfile(hello world): Failed to open stream: No such file or directory in %s on line %d
bool(false)
