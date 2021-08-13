--TEST--
gzfile() with various invalid params
--EXTENSIONS--
zlib
--FILE--
<?php

var_dump(gzfile("nonexistent_file_gzfile",1));

var_dump(gzfile(__DIR__."/004私はガラスを食べられます.txt.gz"));
var_dump(gzfile(__DIR__."/004私はガラスを食べられます.txt.gz", 1));

echo "Done\n";
?>
--EXPECTF--
Warning: gzfile(nonexistent_file_gzfile): Failed to open stream: No such file or directory in %s on line %d
bool(false)
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
Done
