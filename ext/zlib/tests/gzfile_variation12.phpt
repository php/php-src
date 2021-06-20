--TEST--
Test function gzfile() by substituting argument 2 with int values.
--EXTENSIONS--
zlib
--FILE--
<?php


$filename = __DIR__."/004.txt.gz";


$variation = array (
    'int 0' => 0,
    'int 1' => 1,
    'int 12345' => 12345,
    'int -12345' => -2345,
    );


foreach ( $variation as $var ) {
  var_dump(gzfile( $filename, $var  ) );
}
?>
--EXPECT--
array(6) {
  [0]=>
  string(36) "When you're taught through feelings
"
  [1]=>
  string(26) "Destiny flying high above
"
  [2]=>
  string(38) "all I know is that you can realize it
"
  [3]=>
  string(18) "Destiny who cares
"
  [4]=>
  string(19) "as it turns around
"
  [5]=>
  string(39) "and I know that it descends down on me
"
}
array(6) {
  [0]=>
  string(36) "When you're taught through feelings
"
  [1]=>
  string(26) "Destiny flying high above
"
  [2]=>
  string(38) "all I know is that you can realize it
"
  [3]=>
  string(18) "Destiny who cares
"
  [4]=>
  string(19) "as it turns around
"
  [5]=>
  string(39) "and I know that it descends down on me
"
}
array(6) {
  [0]=>
  string(36) "When you're taught through feelings
"
  [1]=>
  string(26) "Destiny flying high above
"
  [2]=>
  string(38) "all I know is that you can realize it
"
  [3]=>
  string(18) "Destiny who cares
"
  [4]=>
  string(19) "as it turns around
"
  [5]=>
  string(39) "and I know that it descends down on me
"
}
array(6) {
  [0]=>
  string(36) "When you're taught through feelings
"
  [1]=>
  string(26) "Destiny flying high above
"
  [2]=>
  string(38) "all I know is that you can realize it
"
  [3]=>
  string(18) "Destiny who cares
"
  [4]=>
  string(19) "as it turns around
"
  [5]=>
  string(39) "and I know that it descends down on me
"
}
