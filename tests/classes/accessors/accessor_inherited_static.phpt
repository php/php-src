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

echo "Done\n";
?>
--EXPECTF--
5
5
Done
