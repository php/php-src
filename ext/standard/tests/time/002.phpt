--TEST--
strtotime() function
--POST--
--GET--
--FILE--
<?php
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
939765600
939765600

2000-01-19
948236400
948236400

2001-12-21
1008889200
1008889200

2001-12-21 12:16
1008933360
1008933360
-1
