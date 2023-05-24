<?php

function empty_loop($n) {
    for ($i = 0; $i < $n; ++$i) {
    }
}

function gethrtime()
{
  $hrtime = hrtime();
  return (($hrtime[0]*1000000000 + $hrtime[1]) / 1000000000);
}

function start_test()
{
  ob_start();
  return gethrtime();
}

function end_test($start, $name, $overhead = null)
{
  global $total;
  global $last_time;
  $end = gethrtime();
  ob_end_clean();
  $last_time = $end-$start;
  $total += $last_time;
  $num = number_format($last_time,3);
  $pad = str_repeat(" ", 24-strlen($name)-strlen($num));
  if (is_null($overhead)) {
    echo $name.$pad.$num."\n";
  } else {
    $num2 = number_format($last_time - $overhead,3);
    echo $name.$pad.$num."    ".$num2."\n";
  }
  ob_start();
  return gethrtime();
}

function total()
{
  global $total;
  $pad = str_repeat("-", 24);
  echo $pad."\n";
  $num = number_format($total,3);
  $pad = str_repeat(" ", 24-strlen("Total")-strlen($num));
  echo "Total".$pad.$num."\n";
}
