--TEST--
PFA variation: variadics and optional args
--FILE--
<?php
function foo(int $day = 1, int $month = 1, int $year = 2005) {
    printf("%04d-%02d-%02d\n", $year, $month, $day);
}

$foo = foo(year: 2006, ...);

echo "# Bound year, pass day:\n";

$foo(2);

$foo = foo(month: 12, ...);

$bar = $foo(year: 2016, ...);

echo "# Bound month, pass day:\n";

$foo(2);

echo "# Bound month, bound year, pass day:\n";

$bar(2);

echo "# Bound month, no args:\n";

$foo();

echo "# Bound month, bound year, no args:\n";

$bar();
?>
--EXPECT--
# Bound year, pass day:
2006-01-02
# Bound month, pass day:
2005-12-02
# Bound month, bound year, pass day:
2016-12-02
# Bound month, no args:
2005-12-01
# Bound month, bound year, no args:
2016-12-01
