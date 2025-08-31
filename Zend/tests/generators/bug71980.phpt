--TEST--
Bug #71980: Decorated/Nested Generator is Uncloseable in Finally
--FILE--
<?php

class Dtor {
    public function __destruct() {
        echo "Dtor\n";
    }
}

function gen1() {
    try {
        foreach ([42, new Dtor] as $value) {
            yield $value;
        }
    } finally {
        echo "Finally\n";
    }
}

function gen2() {
    try {
        var_dump(new Dtor, yield);
    } finally {
        echo "Finally\n";
    }
}

$gen = gen1();
$gen->rewind();
unset($gen);

$gen = gen2();
$gen->rewind();
unset($gen);

?>
--EXPECT--
Dtor
Finally
Dtor
Finally
