<?php

function compare_function($var1, $var2) {
  if ($var1 == $var2) {
    return 0;
  } else if ($var1 < $var2) {
    return -1;
  } else {
    return 1;
  }
}

?>