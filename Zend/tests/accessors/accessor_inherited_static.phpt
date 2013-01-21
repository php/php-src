--TEST--
ZE2 Tests that an inherited static accessor works properlty
--FILE--
<?php

class base {
    /** Test Doc Comment */
	public static $z {
		get;
		set;
	}
}

class sub extends base {
}

base::$z = 5;
echo base::$z."\r\n";
echo sub::$z."\r\n";

/*********************************************************************************
 * Static accessors as a feature for first release was shelved, the lines below 
 *	are what this test should output when static accessors are written:
 *********************************************************************************
5
5
==DONE==
 */

?>
==DONE==
--EXPECTF--
Fatal error: Cannot define static accessor %s, not supported at this time in %s on line %d
