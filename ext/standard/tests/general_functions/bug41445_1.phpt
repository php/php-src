--TEST--
Bug #41445 (parse_ini_file() function parses octal numbers in section names) - 2 
--FILE--
<?php

$file = dirname(__FILE__)."/bug41445.ini";

$data = <<<DATA
[2454.33]
09 = yes

[9876543]
098765434567876543 = yes

[09876543]
987654345678765432456798765434567876543 = yes
DATA;

file_put_contents($file, $data);

var_dump(parse_ini_file($file, TRUE));
var_dump(parse_ini_file($file));

@unlink($file);

echo "Done\n";
?>
--EXPECTF--	
array(3) {
  ["2454.33"]=>
  array(1) {
    ["09"]=>
    string(1) "1"
  }
  [9876543]=>
  array(1) {
    ["098765434567876543"]=>
    string(1) "1"
  }
  ["09876543"]=>
  array(1) {
    ["987654345678765432456798765434567876543"]=>
    string(1) "1"
  }
}
array(3) {
  ["09"]=>
  string(1) "1"
  ["098765434567876543"]=>
  string(1) "1"
  ["987654345678765432456798765434567876543"]=>
  string(1) "1"
}
Done
