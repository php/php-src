--TEST--
stristr() function
--FILE--
<?php
	var_dump(stristr());
	var_dump(stristr(array(), ""));
	var_dump(stristr("", array()));
	var_dump(stristr(array(), array()));

	var_dump(stristr("tEsT sTrInG", "tEsT"));
	var_dump(stristr("tEsT sTrInG", "stRiNg"));
	var_dump(stristr("tEsT sTrInG", "stRiN"));
	var_dump(stristr("tEsT sTrInG", "t S"));
	var_dump(stristr("tEsT sTrInG", "g"));
	var_dump(md5(stristr("te".chr(0)."st", chr(0))));
	var_dump(@stristr("", ""));
	var_dump(@stristr("a", ""));
	var_dump(@stristr("", "a"));
	var_dump(md5(@stristr("\\\\a\\", "\\a")));
	var_dump(stristr("tEsT sTrInG", " ", false));
	var_dump(stristr("tEsT sTrInG", " ", true));
?>
--EXPECTF--
Warning: stristr() expects at least 2 parameters, 0 given in %s on line %d
NULL

Notice: Array to string conversion in %s on line %d

Warning: stristr(): Empty delimiter in %s on line %d
bool(false)
bool(false)

Notice: Array to string conversion in %s on line %d
bool(false)
unicode(11) "tEsT sTrInG"
unicode(6) "sTrInG"
unicode(6) "sTrInG"
unicode(8) "T sTrInG"
unicode(1) "G"
unicode(32) "7272696018bdeb2c9a3f8d01fc2a9273"
bool(false)
bool(false)
bool(false)
unicode(32) "6ec19f52f0766c463f3bb240f4396913"
unicode(7) " sTrInG"
unicode(4) "tEsT"
