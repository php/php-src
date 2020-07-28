--TEST--
Test function gzfile() by substituting argument 1 with emptyUnsetUndefNull values.
--SKIPIF--
<?php
if (!extension_loaded('zlib')) die ('skip zlib extension not available in this build');
?>
--FILE--
<?php


$use_include_path = false;


$unset_var = 10;
unset($unset_var);

$variation = array(
  'unset var' => @$unset_var,
  'undefined var' => @$undefined_var,
  'empty string DQ' => "",
  'empty string SQ' => '',
  'uppercase NULL' => NULL,
  'lowercase null' => null,
  );


foreach ( $variation as $var ) {
    try {
        var_dump(gzfile( $var ,  $use_include_path ) );
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
}
?>
--EXPECT--
Path cannot be empty
Path cannot be empty
Path cannot be empty
Path cannot be empty
Path cannot be empty
Path cannot be empty
