--TEST--
Test allowing semi-reserved identifiers in more contexts
--FILE--
<?php

// When php's syntax rules change to allow keywords in more contexts,
// those keywords will start being forbidden in those specific contexts.

// Previously not allowed
trait Or {
}

// Previously not allowed since 8.0
class Match {
    use \Or;
    const OR = 123;
}

final class Readonly {
}

// Previously not allowed since 7.4
function fn() {
    echo "in fn\n";
}
\fn();
var_dump(\Match::OR);
var_dump(get_class(new \Match()));
var_dump(get_class(new \Readonly()));
?>
--EXPECT--
in fn
int(123)
string(5) "Match"
string(8) "Readonly"
