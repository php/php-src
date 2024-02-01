--TEST--
Nullsafe property constant expression
--FILE--
<?php

class Printer {
    public function __construct() {
        echo "Printer\n";
    }
}

const A = (null)?->test;
var_dump(A);

const B = (null)?->test->test;
var_dump(B);

const C = (null)->test?->test;
var_dump(C);

const D = (null)?->test['test'];
var_dump(D);

const E = (null)['test']?->test;
var_dump(E);

const F = (null)?->{new Printer};
var_dump(F);

const G = (null)?->test + (new Printer ? 1 : 0);
var_dump(G);

?>
--EXPECTF--
NULL
NULL

Warning: Attempt to read property "test" on null in %s on line %d
NULL
NULL

Warning: Trying to access array offset on value of type null in %s on line %d
NULL
NULL
Printer
int(1)
