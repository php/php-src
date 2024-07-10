--TEST--
Test printf INF/-INF/NAN rendering
--FILE--
<?php

$values = [
    INF,
    abs(log(0)),
    -INF,
    log(0),
    NAN,
    -NAN,
    sqrt(-1)
];

$valuesShort = [
    INF,
    -INF,
    NAN,
    -NAN
];

// Test each float format
foreach ($values as $value) {
    printf("#%1\$e# #%1\$E# #%1\$f# #%1\$F# #%1\$g# #%1\$G# #%1\$h# #%1\$H#\n", $value);
}

// Test padding and sign
vprintf(
    "#%+f# #%+f# #%+f# #%+f#\n",
    $valuesShort
);
vprintf(
    "#%10f# #%10f# #%10f# #%10f#\n",
    $valuesShort
);
vprintf(
    "#%+10f# #%+10f# #%+10f# #%+10f#\n",
    $valuesShort
);
vprintf(
    "#%-10f# #%-10f# #%-10f# #%-10f#\n",
    $valuesShort
);
vprintf(
    "#%-+10f# #%-+10f# #%-+10f# #%-+10f#\n",
    $valuesShort
);
vprintf(
    "#%010f# #%010f# #%010f# #%010f#\n",
    $valuesShort
);
vprintf(
    "#%+010f# #%+010f# #%+010f# #%+010f#\n",
    $valuesShort
);
vprintf(
    "#%-010f# #%-010f# #%-010f# #%-010f#\n",
    $valuesShort
);
vprintf(
    "#%-+010f# #%-+010f# #%-+010f# #%-+010f#\n",
    $valuesShort
);
vprintf(
    "#%'n10f# #%'n10f# #%'n10f# #%'n10f#\n",
    $valuesShort
);
vprintf(
    "#%+'n10f# #%+'n10f# #%+'n10f# #%+'n10f#\n",
    $valuesShort
);
vprintf(
    "#%-'n10f# #%-'n10f# #%-'n10f# #%-'n10f#\n",
    $valuesShort
);
vprintf(
    "#%-+'n10f# #%-+'n10f# #%-+'n10f# #%-+'n10f#\n",
    $valuesShort
);
vprintf(
    "#% 10f# #% 10f# #% 10f# #% 10f#\n",
    $valuesShort
);
vprintf(
    "#%+ 10f# #%+ 10f# #%+ 10f# #%+ 10f#\n",
    $valuesShort
);
vprintf(
    "#%- 10f# #%- 10f# #%- 10f# #%- 10f#\n",
    $valuesShort
);
vprintf(
    "#%-+ 10f# #%-+ 10f# #%-+ 10f# #%-+ 10f#\n",
    $valuesShort
);

?>
--EXPECT--
#INF# #INF# #INF# #INF# #INF# #INF# #INF# #INF#
#INF# #INF# #INF# #INF# #INF# #INF# #INF# #INF#
#-INF# #-INF# #-INF# #-INF# #-INF# #-INF# #-INF# #-INF#
#-INF# #-INF# #-INF# #-INF# #-INF# #-INF# #-INF# #-INF#
#NAN# #NAN# #NAN# #NAN# #NAN# #NAN# #NAN# #NAN#
#NAN# #NAN# #NAN# #NAN# #NAN# #NAN# #NAN# #NAN#
#NAN# #NAN# #NAN# #NAN# #NAN# #NAN# #NAN# #NAN#
#+INF# #-INF# #NAN# #NAN#
#       INF# #      -INF# #       NAN# #       NAN#
#      +INF# #      -INF# #       NAN# #       NAN#
#INF       # #-INF      # #NAN       # #NAN       #
#+INF      # #-INF      # #NAN       # #NAN       #
#       INF# #      -INF# #       NAN# #       NAN#
#      +INF# #      -INF# #       NAN# #       NAN#
#INF       # #-INF      # #NAN       # #NAN       #
#+INF      # #-INF      # #NAN       # #NAN       #
#       INF# #      -INF# #       NAN# #       NAN#
#      +INF# #      -INF# #       NAN# #       NAN#
#INF       # #-INF      # #NAN       # #NAN       #
#+INF      # #-INF      # #NAN       # #NAN       #
#       INF# #      -INF# #       NAN# #       NAN#
#      +INF# #      -INF# #       NAN# #       NAN#
#INF       # #-INF      # #NAN       # #NAN       #
#+INF      # #-INF      # #NAN       # #NAN       #
