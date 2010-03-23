--TEST--
jddayofweek()
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
foreach (array(2440588, 2452162, 2453926, -1000) as $jd) {
  echo "### JD $jd ###\n";
  for ($mode = 0; $mode <= 2; $mode++) {
    echo "--- mode $mode ---\n";
    for ($offset = 0; $offset <= 7; $offset++) {
      echo jddayofweek($jd + $offset, $mode). "\n";
    }
  }
}
?>
--EXPECT--
### JD 2440588 ###
--- mode 0 ---
4
5
6
0
1
2
3
4
--- mode 1 ---
Thursday
Friday
Saturday
Sunday
Monday
Tuesday
Wednesday
Thursday
--- mode 2 ---
Thu
Fri
Sat
Sun
Mon
Tue
Wed
Thu
### JD 2452162 ###
--- mode 0 ---
0
1
2
3
4
5
6
0
--- mode 1 ---
Sunday
Monday
Tuesday
Wednesday
Thursday
Friday
Saturday
Sunday
--- mode 2 ---
Sun
Mon
Tue
Wed
Thu
Fri
Sat
Sun
### JD 2453926 ###
--- mode 0 ---
0
1
2
3
4
5
6
0
--- mode 1 ---
Sunday
Monday
Tuesday
Wednesday
Thursday
Friday
Saturday
Sunday
--- mode 2 ---
Sun
Mon
Tue
Wed
Thu
Fri
Sat
Sun
### JD -1000 ###
--- mode 0 ---
2
3
4
5
6
0
1
2
--- mode 1 ---
Tuesday
Wednesday
Thursday
Friday
Saturday
Sunday
Monday
Tuesday
--- mode 2 ---
Tue
Wed
Thu
Fri
Sat
Sun
Mon
Tue

