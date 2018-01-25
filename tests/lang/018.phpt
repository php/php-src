--TEST--
eval() test
--FILE--
<?php

error_reporting(0);

$message = "echo \"hey\n\";";

for ($i=0; $i<10; $i++) {
  eval($message);
  echo $i."\n";
}
--EXPECT--
hey
0
hey
1
hey
2
hey
3
hey
4
hey
5
hey
6
hey
7
hey
8
hey
9
