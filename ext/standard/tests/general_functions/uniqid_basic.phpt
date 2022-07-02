--TEST--
Test uniqid() function : basic functionality
--FILE--
<?php
echo "*** Testing uniqid() : basic functionality ***\n";

echo "\nuniqid() without a prefix\n";
var_dump(uniqid());
var_dump(uniqid('', true));
var_dump(uniqid('', false));
echo "\n\n";

echo "uniqid() with a prefix\n";

// Use a fixed prefix so we can ensure length of o/p id is fixed
$prefix = array (
                99999,
                "99999",
                10.5e2,
                true,
                false
                );

for ($i = 0; $i < count($prefix); $i++) {
    var_dump(uniqid($prefix[$i]));
    var_dump(uniqid($prefix[$i], true));
    var_dump(uniqid($prefix[$i], false));
    echo "\n";
}

?>
--EXPECTF--
*** Testing uniqid() : basic functionality ***

uniqid() without a prefix
string(13) "%s"
string(23) "%s.%s"
string(13) "%s"


uniqid() with a prefix
string(18) "99999%s"
string(28) "99999%s.%s"
string(18) "99999%s"

string(18) "99999%s"
string(28) "99999%s.%s"
string(18) "99999%s"

string(17) "1050%s"
string(27) "1050%s.%s"
string(17) "1050%s"

string(14) "1%s"
string(24) "1%s.%s"
string(14) "1%s"

string(13) "%s"
string(23) "%s.%s"
string(13) "%s"
