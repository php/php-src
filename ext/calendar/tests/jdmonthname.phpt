--TEST--
jddayofweek()
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
foreach (array(2440588, 2452162, 2453926) as $jd) {
  echo "### JD $jd ###\n";
  for ($mode = 0; $mode <= 6; $mode++) {
    echo "--- mode $mode ---\n";
    for ($offset = 0; $offset <= 12; $offset++) {
      echo jddayofweek($jd + $offset * 30, $mode). "\n";
    }
  }
}
?>
--EXPECT--
### JD 2440588 ###
--- mode 0 ---
4
6
1
3
5
0
2
4
6
1
3
5
0
--- mode 1 ---
Thursday
Saturday
Monday
Wednesday
Friday
Sunday
Tuesday
Thursday
Saturday
Monday
Wednesday
Friday
Sunday
--- mode 2 ---
Thu
Sat
Mon
Wed
Fri
Sun
Tue
Thu
Sat
Mon
Wed
Fri
Sun
--- mode 3 ---
4
6
1
3
5
0
2
4
6
1
3
5
0
--- mode 4 ---
4
6
1
3
5
0
2
4
6
1
3
5
0
--- mode 5 ---
4
6
1
3
5
0
2
4
6
1
3
5
0
--- mode 6 ---
4
6
1
3
5
0
2
4
6
1
3
5
0
### JD 2452162 ###
--- mode 0 ---
0
2
4
6
1
3
5
0
2
4
6
1
3
--- mode 1 ---
Sunday
Tuesday
Thursday
Saturday
Monday
Wednesday
Friday
Sunday
Tuesday
Thursday
Saturday
Monday
Wednesday
--- mode 2 ---
Sun
Tue
Thu
Sat
Mon
Wed
Fri
Sun
Tue
Thu
Sat
Mon
Wed
--- mode 3 ---
0
2
4
6
1
3
5
0
2
4
6
1
3
--- mode 4 ---
0
2
4
6
1
3
5
0
2
4
6
1
3
--- mode 5 ---
0
2
4
6
1
3
5
0
2
4
6
1
3
--- mode 6 ---
0
2
4
6
1
3
5
0
2
4
6
1
3
### JD 2453926 ###
--- mode 0 ---
0
2
4
6
1
3
5
0
2
4
6
1
3
--- mode 1 ---
Sunday
Tuesday
Thursday
Saturday
Monday
Wednesday
Friday
Sunday
Tuesday
Thursday
Saturday
Monday
Wednesday
--- mode 2 ---
Sun
Tue
Thu
Sat
Mon
Wed
Fri
Sun
Tue
Thu
Sat
Mon
Wed
--- mode 3 ---
0
2
4
6
1
3
5
0
2
4
6
1
3
--- mode 4 ---
0
2
4
6
1
3
5
0
2
4
6
1
3
--- mode 5 ---
0
2
4
6
1
3
5
0
2
4
6
1
3
--- mode 6 ---
0
2
4
6
1
3
5
0
2
4
6
1
3