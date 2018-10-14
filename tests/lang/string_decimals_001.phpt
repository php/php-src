--TEST--
String conversion with multiple decimal points
--FILE--
<?php
function test($str) {
  echo "\n--> Testing $str:\n";
  var_dump((int)$str);
  var_dump((float)$str);
  var_dump($str > 0);
}

test("..9");
test(".9.");
test("9..");
test("9.9.");
test("9.9.9");
?>
===DONE===
--EXPECTF--
--> Testing ..9:
int(0)
float(0)
bool(false)

--> Testing .9.:
int(0)
float(0.9)
bool(true)

--> Testing 9..:
int(9)
float(9)
bool(true)

--> Testing 9.9.:
int(9)
float(9.9)
bool(true)

--> Testing 9.9.9:
int(9)
float(9.9)
bool(true)
===DONE===
