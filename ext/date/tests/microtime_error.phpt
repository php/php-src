--TEST--
Test wrong number of arguments for microtime()
--FILE--
<?php
/*
 * proto mixed microtime([bool get_as_float])
 * Function is implemented in ext/standard/microtime.c
*/

$opt_arg_0 = true;
$extra_arg = 1;

echo "\n-- Too many arguments --\n";
var_dump(microtime($opt_arg_0, $extra_arg));


echo "\n-- Bad Arg types --\n";

$bad_args = array(null,
				  1.5,
				  "hello",
				  array('k'=>'v', array(0)),
				  new stdClass,
				  1);
foreach ($bad_args as $bad_arg) {
	echo "\n--> bad arg: ";
	var_dump($bad_arg);
	var_dump(microtime($bad_arg));
}

?>
===DONE===
--EXPECTF--
-- Too many arguments --

Warning: microtime() expects at most 1 parameter, 2 given in %s on line 11
NULL

-- Bad Arg types --

--> bad arg: NULL
string(%d) "%s %s"

--> bad arg: float(1.5)
float(%s)

--> bad arg: string(5) "hello"
float(%s)

--> bad arg: array(2) {
  ["k"]=>
  string(1) "v"
  [0]=>
  array(1) {
    [0]=>
    int(0)
  }
}

Warning: microtime() expects parameter 1 to be boolean, array given in %s on line 25
NULL

--> bad arg: object(stdClass)#%d (0) {
}

Warning: microtime() expects parameter 1 to be boolean, object given in %s on line 25
NULL

--> bad arg: int(1)
float(%s)
===DONE===
