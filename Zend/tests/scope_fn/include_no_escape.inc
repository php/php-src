<?php
$x = 7;
$fn = fn() { return $x; };
echo "in include, fn() = ", $fn(), "\n";
