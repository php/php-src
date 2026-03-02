--TEST--
Bug #71441 (Typehinted Generator with return in try/finally crashes)
--FILE--
<?php

$num = 2000; /* to be sure to be in wild memory */
$add = str_repeat("1 +", $num);
$gen = (eval(<<<PHP
return function (): \Generator {
    try {
        \$a = 1;
        \$foo = \$a + $add \$a;
        return yield \$foo;
    } finally {
        print "Ok\n";
    }
};
PHP
))();
var_dump($gen->current());
$gen->send("Success");
var_dump($gen->getReturn());

?>
--EXPECT--
int(2002)
Ok
string(7) "Success"
