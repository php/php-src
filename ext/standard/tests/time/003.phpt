--TEST--
Check for mktime with out-of-range parameters
--SKIPIF--
--POST--
--GET--
--FILE--
<?php 
  # MacOS/X libc implementation doesn't treat out-of-range values
  # the same way other unices do (Bug# 10686) so some extra code
  # was added to datetime.c to take care of this 
	echo date("Y-m-d", mktime( 12, 0, 0, 3,  0, 2000)) ."\n";
	echo date("Y-m-d", mktime( 12, 0, 0, 3, -1, 2000)) ."\n";
	echo date("Y-m-d", mktime( 12, 0, 0, 2, 29, 2000)) ."\n";
	echo date("Y-m-d", mktime( 12, 0, 0, 3,  0, 2001)) ."\n";
	echo date("Y-m-d", mktime( 12, 0, 0, 2, 29, 2001)) ."\n";
	echo date("Y-m-d", mktime( 12, 0, 0, 0,  0, 2000)) ."\n";

  putenv("TZ=GST-1GDT");
  echo date("Y-m-d H:i:s", mktime(12,0,0,3,+90,2000,-1))."\n";
  echo date("Y-m-d H:i:s", mktime(12,0,0,3,+90,2000,0))."\n";
  echo date("Y-m-d H:i:s", mktime(12,0,0,3,+90,2000,1))."\n";
  echo date("Y-m-d H:i:s", mktime(12,0,0,5,-90,2000,-1))."\n";
  echo date("Y-m-d H:i:s", mktime(12,0,0,5,-90,2000,0))."\n";
  echo date("Y-m-d H:i:s", mktime(12,0,0,5,-90,2000,1))."\n";
  echo date("Y-m-d H:i:s", mktime(12,0,0,5,-1,2000,-1))."\n";
  echo date("Y-m-d H:i:s", mktime(12,0,0,5,-1,2000,0))."\n";
  echo date("Y-m-d H:i:s", mktime(12,0,0,5,-1,2000,1))."\n";
?>
--EXPECT--
2000-02-29
2000-02-28
2000-02-29
2001-02-28
2001-03-01
1999-11-30
2000-05-29 12:00:00
2000-05-29 13:00:00
2000-05-29 12:00:00
2000-01-31 12:00:00
2000-01-31 12:00:00
2000-01-31 11:00:00
2000-04-29 12:00:00
2000-04-29 13:00:00
2000-04-29 12:00:00
