--TEST--
strtotime() function
--POST--
--GET--
--FILE--
<?php
	putenv("TZ=GMT");

	echo "1999-10-13\n";
	echo strtotime ("1999-10-13")."\n";
	echo strtotime ("Oct 13  1999")."\n\n";

	echo "2000-01-19\n";
	echo strtotime ("2000-01-19")."\n";
	echo strtotime ("Jan 19  2000")."\n\n";

	echo "2001-12-21\n";
	echo strtotime ("2001-12-21")."\n";
	echo strtotime ("Dec 21  2001")."\n\n";

	echo "2001-12-21 12:16\n";
	echo strtotime ("2001-12-21 12:16")."\n";
	echo strtotime ("Dec 21 2001 12:16")."\n";
	echo strtotime ("Dec 21  12:16")."\n";
?>
--EXPECT--
1999-10-13
939772800
939772800

2000-01-19
948240000
948240000

2001-12-21
1008892800
1008892800

2001-12-21 12:16
1008936960
1008936960
-1
