--TEST--
Test function readgzfile() by substituting agument 2 with boolean values.
--SKIPIF--
<?php
if (!extension_loaded('zlib')) die ('skip zlib extension not available in this build');
?>
--FILE--
<?php


$filename = dirname(__FILE__)."/004.txt.gz";

$variation = array(
  'lowercase true' => true,
  'lowercase false' =>false,
  'uppercase TRUE' =>TRUE,
  'uppercase FALSE' =>FALSE,
  );


foreach ( $variation as $var ) {
  var_dump(readgzfile( $filename, $var  ) );
}
?>
===DONE===
--EXPECT--
When you're taught through feelings
Destiny flying high above
all I know is that you can realize it
Destiny who cares
as it turns around
and I know that it descends down on me
int(176)
When you're taught through feelings
Destiny flying high above
all I know is that you can realize it
Destiny who cares
as it turns around
and I know that it descends down on me
int(176)
When you're taught through feelings
Destiny flying high above
all I know is that you can realize it
Destiny who cares
as it turns around
and I know that it descends down on me
int(176)
When you're taught through feelings
Destiny flying high above
all I know is that you can realize it
Destiny who cares
as it turns around
and I know that it descends down on me
int(176)
===DONE===