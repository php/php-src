--TEST--
Test function readgzfile() by substituting argument 1 with string values.
--EXTENSIONS--
zlib
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
--EXPECTF--
Warning: readgzfile(string): Failed to open stream: No such file or directory in %s on line %d
bool(false)

Warning: readgzfile(string): Failed to open stream: No such file or directory in %s on line %d
bool(false)

Warning: readgzfile(sTrInG): Failed to open stream: No such file or directory in %s on line %d
bool(false)

Warning: readgzfile(hello world): Failed to open stream: No such file or directory in %s on line %d
bool(false)
