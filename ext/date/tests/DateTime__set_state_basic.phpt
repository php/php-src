--TEST--
Basic test for __set_state() method
--CREDITS--
Gabriel da Gama <gabriel@gabrielgama.com.br>
# PHPDublin PHPTestFest 2017
--FILE--
<?php
date_default_timezone_set('Europe/Dublin');
echo "*** Testing __set_state on DateTime objects ***\n";
$dateTime = new DateTime('2017-10-18 19:50:00');
eval('$b = ' . var_export($dateTime, true) . ';');
var_dump($b);
?>
--EXPECTF--
*** Testing __set_state on DateTime objects ***
object(DateTime)#2 (3) {
  ["date"]=>
  string(26) "2017-10-18 19:50:00.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/Dublin"
}
