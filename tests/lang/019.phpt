--TEST--
eval() test
--FILE--
<?php

eval("function test() { echo \"hey, this is a function inside an eval()!\\n\"; }");

$i=0;
while ($i<10) {
  eval("echo \"hey, this is a regular echo'd eval()\\n\";");
  test();
  $i++;
}

eval('-');
--EXPECTF--
hey, this is a regular echo'd eval()
hey, this is a function inside an eval()!
hey, this is a regular echo'd eval()
hey, this is a function inside an eval()!
hey, this is a regular echo'd eval()
hey, this is a function inside an eval()!
hey, this is a regular echo'd eval()
hey, this is a function inside an eval()!
hey, this is a regular echo'd eval()
hey, this is a function inside an eval()!
hey, this is a regular echo'd eval()
hey, this is a function inside an eval()!
hey, this is a regular echo'd eval()
hey, this is a function inside an eval()!
hey, this is a regular echo'd eval()
hey, this is a function inside an eval()!
hey, this is a regular echo'd eval()
hey, this is a function inside an eval()!
hey, this is a regular echo'd eval()
hey, this is a function inside an eval()!

Parse error: syntax error, unexpected %s in %s019.php(12) : eval()'d code on line 1
