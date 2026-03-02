--TEST--
#[\Override]: Redeclared trait method with interface.
--FILE--
<?php

interface I {
    public function i(): string;
}

trait T {
    public function i(): string {
        return 'T';
    }
}

class C implements I {
    use T;

    #[\Override]
    public function i(): string {
        return 'C';
    }
}

var_dump((new C())->i());

echo "Done";

?>
--EXPECT--
string(1) "C"
Done
