--TEST--
GH-23204 (Use-after-free in implode() when __toString() destroys the array)
--FILE--
<?php
class Unset_ implements Stringable {
    public function __toString(): string {
        global $a;
        $a = null;
        return "X";
    }
}

$a = [new Unset_, 2, 3, 4];
echo "destroyed: ", implode(",", $a), "\n";
var_dump($a);

class Append implements Stringable {
    public function __toString(): string {
        global $b;
        $b[] = str_repeat("y", 32);
        return "X";
    }
}

$b = [new Append, 2, 3, 4];
echo "appended: ", implode(",", $b), "\n";
echo "count: ", count($b), "\n";

class Boom implements Stringable {
    public function __toString(): string {
        global $c;
        $c = null;
        throw new Exception("boom");
    }
}

$c = [new Boom, 2, 3, 4];
try {
    implode(",", $c);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
destroyed: X,2,3,4
NULL
appended: X,2,3,4
count: 5
boom
