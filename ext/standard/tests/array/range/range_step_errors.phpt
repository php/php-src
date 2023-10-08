--TEST--
range() programattic errors for the $step parameter
--INI--
precision=14
--FILE--
<?php
echo "Step cannot be 0\n";
try {
    var_dump( range(1.0, 7.0, 0.0) );
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump( range(1, 7, 0) );
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump( range('A', 'H', 0) );
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump( range('A', 'H', 0.0) );
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}
echo "Step cannot be INF\n";
try {
    var_dump( range(1.0, 7.0, 10.0**400) );
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump( range(1, 7, 10.0**400) );
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump( range('A', 'H', 10.0**400) );
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}
echo "Step cannot be NAN\n";
try {
    var_dump( range(1.0, 7.0, fdiv(0, 0)) );
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump( range(1, 7, fdiv(0, 0)) );
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump( range('A', 'H', fdiv(0, 0)) );
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}

echo "Step must be within the range of input parameters\n";
echo "-- Testing ( (low < high) && (high-low < step) ) --\n";
try {
    var_dump( range(1.0, 7.0, 6.5) );
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}

echo "-- Testing ( (low > high) && (low-high < step) ) --\n";
try {
    var_dump( range(7.0, 1.0, 6.5) );
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}

echo "-- Testing ( (low < high) && (high-low < step) ) for characters --\n";
try {
    var_dump(range('a', 'z', 100));
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}

echo "-- Testing ( (low > high) && (low-high < step) ) for characters --\n";
try {
    var_dump(range('z', 'a', 100));
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}

echo "Step must not be negative for increasing ranges\n";
try {
    var_dump(range('a', 'c', -1));
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(range(1, 3, -1));
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(range(1.5, 3.5, -1.5));
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Step cannot be 0
range(): Argument #3 ($step) cannot be 0
range(): Argument #3 ($step) cannot be 0
range(): Argument #3 ($step) cannot be 0
range(): Argument #3 ($step) cannot be 0
Step cannot be INF
range(): Argument #3 ($step) must be a finite number, INF provided
range(): Argument #3 ($step) must be a finite number, INF provided
range(): Argument #3 ($step) must be a finite number, INF provided
Step cannot be NAN
range(): Argument #3 ($step) must be a finite number, NAN provided
range(): Argument #3 ($step) must be a finite number, NAN provided
range(): Argument #3 ($step) must be a finite number, NAN provided
Step must be within the range of input parameters
-- Testing ( (low < high) && (high-low < step) ) --
range(): Argument #3 ($step) must be less than the range spanned by argument #1 ($start) and argument #2 ($end)
-- Testing ( (low > high) && (low-high < step) ) --
range(): Argument #3 ($step) must be less than the range spanned by argument #1 ($start) and argument #2 ($end)
-- Testing ( (low < high) && (high-low < step) ) for characters --
range(): Argument #3 ($step) must be less than the range spanned by argument #1 ($start) and argument #2 ($end)
-- Testing ( (low > high) && (low-high < step) ) for characters --
range(): Argument #3 ($step) must be less than the range spanned by argument #1 ($start) and argument #2 ($end)
Step must not be negative for increasing ranges
range(): Argument #3 ($step) must be greater than 0 for increasing ranges
range(): Argument #3 ($step) must be greater than 0 for increasing ranges
range(): Argument #3 ($step) must be greater than 0 for increasing ranges
