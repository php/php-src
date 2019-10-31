--TEST--
Test wrong number of arguments for microtime()
--FILE--
<?php
/*
 * proto mixed microtime([bool get_as_float])
 * Function is implemented in ext/standard/microtime.c
*/

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
	try {
		var_dump(microtime($bad_arg));
	} catch (TypeError $e) {
		echo $e->getMessage(), "\n";
	}
}

?>
--EXPECTF--
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
microtime() expects parameter 1 to be bool, array given

--> bad arg: object(stdClass)#%d (0) {
}
microtime() expects parameter 1 to be bool, object given

--> bad arg: int(1)
float(%s)
