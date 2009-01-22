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

Warning: microtime() expects at most 1 parameter, 2 given in %s on line %d
NULL

-- Bad Arg types --

--> bad arg: NULL
unicode(%d) "%s"

--> bad arg: float(1.5)
float(%f)

--> bad arg: unicode(5) "hello"
float(%f)

--> bad arg: array(2) {
  [u"k"]=>
  unicode(1) "v"
  [0]=>
  array(1) {
    [0]=>
    int(0)
  }
}

Warning: microtime() expects parameter 1 to be boolean, array given in %s on line %d
NULL

--> bad arg: object(stdClass)#1 (0) {
}

Warning: microtime() expects parameter 1 to be boolean, object given in %s on line %d
NULL

--> bad arg: int(1)
float(%f)
===DONE===