<html>
<head>
<?php
/* the point of this file is to intensively test various aspects of
 * the parser.  right now, each test focuses in one aspect only
 * (e.g. variable aliasing, arithmetic operator, various control
 * structures), while trying to combine code from other parts of the
 * parser as well.
 */
?>

*** Testing assignments and variable aliasing: ***<br>
<?php
  /* This test tests assignments to variables using other variables as variable-names */
  $a = "b";
  $$a = "test";
  $$$a = "blah";
  ${$$$a}["associative arrays work too"] = "this is nifty";
?>
This should read "blah": <?php echo "$test<br>\n"; ?>
This should read "this is nifty": <?php echo $blah[$test="associative arrays work too"]."<br>\n"; ?>
*************************************************<br>

*** Testing integer operators ***<br>
<?php
  /* test just about any operator possible on $i and $j (ints) */
  $i = 5;
  $j = 3;
?>
Correct result - 8:  <?php echo $i+$j; ?><br>
Correct result - 8:  <?php echo $i+$j; ?><br>
Correct result - 2:  <?php echo $i-$j; ?><br>
Correct result - -2:  <?php echo $j-$i; ?><br>
Correct result - 15:  <?php echo $i*$j; ?><br>
Correct result - 15:  <?php echo $j*$i; ?><br>
Correct result - 2:  <?php echo $i%$j; ?><br>
Correct result - 3:  <?php echo $j%$i; ?><br>
*********************************<br>

*** Testing real operators ***<br>
<?php
  /* test just about any operator possible on $i and $j (floats) */
  $i = 5.0;
  $j = 3.0;
?>
Correct result - 8:  <?php echo $i+$j; ?><br>
Correct result - 8:  <?php echo $i+$j; ?><br>
Correct result - 2:  <?php echo $i-$j; ?><br>
Correct result - -2:  <?php echo $j-$i; ?><br>
Correct result - 15:  <?php echo $i*$j; ?><br>
Correct result - 15:  <?php echo $j*$i; ?><br>
Correct result - 2:  <?php echo $i%$j; ?><br>
Correct result - 3:  <?php echo $j%$i; ?><br>
*********************************<br>

*** Testing if/elseif/else control ***<br>

<?php
/* sick if/elseif/else test by Andi :) */
$a = 5;
if ($a == "4") {
    echo "This "." does "."  not "." work<br>\n";
} elseif ($a == "5") {
    echo "This "." works<br>\n";
    $a = 6;
    if ("andi" == ($test = "andi")) {
        echo "this_still_works<br>\n";
    } elseif (1) {
        echo "should_not_print<br>\n";
    } else {
            echo "should_not_print<br>\n";
    }
        if (44 == 43) {
        echo "should_not_print<br>\n";
    } else {
        echo "should_print<br>\n";
    }
} elseif ($a == 6) {
    echo "this "."broken<br>\n";
    if (0) {
        echo "this_should_not_print<br>\n";
    } else {
        echo "TestingDanglingElse_This_Should_not_print<br>\n";
    }
} else {
    echo "This "."does "." not"." work<br>\n";
}
?>


*** Seriously nested if's test ***<br>
** spelling correction by kluzz **
<?php
/* yet another sick if/elseif/else test by Zeev */
$i=$j=0;
echo "Only two lines of text should follow:<br>\n";
if (0) { /* this code is not supposed to be executed */
  echo "hmm, this shouldn't be displayed #1<br>\n";
  $j++;
  if (1) {
    $i
+=
 $j;
    if (0) {
      $j = ++$i;
      if (1) {
        $j *= $i;
        echo "damn, this shouldn't be displayed<br>\n";
      } else {
        $j /= $i;
        ++$j;
        echo "this shouldn't be displayed either<br>\n";
      }
    } elseif (1) {
      $i++; $j++;
      echo "this isn't supposed to be displayed<br>\n";
    }
  } elseif (0) {
    $i++;
    echo "this definitely shouldn't be displayed<br>\n";
  } else {
    --$j;
    echo "and this too shouldn't be displayed<br>\n";
    while ($j>0) {
      $j--;
    }
  }
} elseif (2-2) {  /* as long as 2-2==0, this isn't supposed to be executed either */
  $i = ++$j;
  echo "hmm, this shouldn't be displayed #2<br>\n";
  if (1) {
    $j = ++$i;
    if (0) {
      $j = $i*2+$j*($i++);
      if (1) {
        $i++;
        echo "damn, this shouldn't be displayed<br>\n";
      } else {
        $j++;
        echo "this shouldn't be displayed either<br>\n";
      }
    } else if (1) {
      ++$j;
      echo "this isn't supposed to be displayed<br>\n";
    }
  } elseif (0) {
    $j++;
    echo "this definitely shouldn't be displayed<br>\n";
  } else {
    $i++;
    echo "and this too shouldn't be displayed<br>\n";
  }
} else {
  $j=$i++;  /* this should set $i to 1, but shouldn't change $j (it's assigned $i's previous values, zero) */
  echo "this should be displayed. should be:  \$i=1, \$j=0.  is:  \$i=$i, \$j=$j<br>\n";
  if (1) {
    $j += ++$i;  /* ++$i --> $i==2,  $j += 2 --> $j==2 */
    if (0) {
      $j += 40;
      if (1) {
        $i += 50;
        echo "damn, this shouldn't be displayed<br>\n";
      } else {
        $j += 20;
        echo "this shouldn't be displayed either<br>\n";
      }
    } else if (1) {
      $j *= $i;  /* $j *= 2  --> $j == 4 */
      echo "this is supposed to be displayed. should be:  \$i=2, \$j=4.  is:  \$i=$i, \$j=$j<br>\n";
      echo "3 loop iterations should follow:<br>\n";
      while ($i<=$j) {
        echo $i++." $j<br>\n";
      }
    }
  } elseif (0) {
    echo "this definitely shouldn't be displayed<br>\n";
  } else {
    echo "and this too shouldn't be displayed<br>\n";
  }
  echo "**********************************<br>\n";
}
?>

*** C-style else-if's ***<br>
<?php
  /* looks like without we even tried, C-style else-if structure works fine! */
  if ($a=0) {
    echo "This shouldn't be displayed<br>\n";
  } else if ($a++) {
    echo "This shouldn't be displayed either<br>\n";
  } else if (--$a) {
    echo "No, this neither<br>\n";
  } else if (++$a) {
    echo "This should be displayed<br>\n";
  } else {
    echo "This shouldn't be displayed at all<br>\n";
  }
?>
*************************<br>

*** WHILE tests ***<br>
<?php
$i=0;
$j=20;
while ($i<(2*$j)) {
  if ($i>$j) {
    echo "$i is greater than $j<br>\n";
  } else if ($i==$j) {
    echo "$i equals $j<br>\n";
  } else {
    echo "$i is smaller than $j<br>\n";
  }
  $i++;
}
?>
*******************<br>


*** Nested WHILEs ***<br>
<?php
$arr_len=3;

$i=0;
while ($i<$arr_len) {
  $j=0;
  while ($j<$arr_len) {
    $k=0;
    while ($k<$arr_len) {
      ${"test$i$j"}[$k] = $i+$j+$k;
      $k++;
    }
    $j++;
  }
  $i++;
}

echo "Each array variable should be equal to the sum of its indices:<br>\n";

$i=0;
while ($i<$arr_len) {
  $j=0;
  while ($j<$arr_len) {
    $k=0;
    while ($k<$arr_len) {
      echo "\${test$i$j}[$k] = ".${"test$i$j"}[$k]."<br>\n";
      $k++;
    }
    $j++;
  }
  $i++;
}
?>
*********************<br>

*** hash test... ***<br>
<?php
/*
$i=0;

while ($i<10000) {
  $arr[$i]=$i;
  $i++;
}

$i=0;
while ($i<10000) {
  echo $arr[$i++]."<br>\n";
}
*/
echo "commented out...";
?>

**************************<br>

*** Hash resizing test ***<br>
<?php
$i = 10;
$a = 'b';
while ($i > 0) {
    $a = $a . 'a';
    echo "$a<br>\n";
    $resize[$a] = $i;
    $i--;
}
$i = 10;
$a = 'b';
while ($i > 0) {
    $a = $a . 'a';
    echo "$a<br>\n";
    echo $resize[$a]."<br>\n";
    $i--;
}
?>
**************************<br>


*** break/continue test ***<br>
<?php
$i=0;

echo "\$i should go from 0 to 2<br>\n";
while ($i<5) {
  if ($i>2) {
    break;
  }
  $j=0;
  echo "\$j should go from 3 to 4, and \$q should go from 3 to 4<br>\n";
  while ($j<5) {
    if ($j<=2) {
      $j++;
      continue;
    }
    echo "  \$j=$j<br>\n";
    for ($q=0; $q<=10; $q++) {
      if ($q<3) {
        continue;
      }
      if ($q>4) {
        break;
      }
      echo "    \$q=$q<br>\n";
    }
    $j++;
  }
  $j=0;
  echo "\$j should go from 0 to 2<br>\n";
  while ($j<5) {
    if ($j>2) {
      $k=0;
      echo "\$k should go from 0 to 2<br>\n";
      while ($k<5) {
        if ($k>2) {
          break 2;
        }
        echo "    \$k=$k<br>\n";
        $k++;
      }
    }
    echo "  \$j=$j<br>\n";
    $j++;
  }
  echo "\$i=$i<br>\n";
  $i++;
}
?>
***********************<br>

*** Nested file include test ***<br>
<?php include("023-2.inc"); ?>
********************************<br>

<?php
{
  echo "Tests completed.<br>\n";  # testing some PHP style comment...
}
?>
