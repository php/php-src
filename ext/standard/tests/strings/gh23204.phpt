--TEST--
GH-23204 (Use-after-free when __toString() destroys the array being read)
--CREDITS--
e1abrador
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
    echo $e::class, ': ', $e->getMessage(), "\n";
}

class UnsetPats implements Stringable {
    public function __toString(): string {
        global $d;
        $d = null;
        return "X";
    }
}

$d = ["aa" => new UnsetPats, "bb" => "2", "cc" => "3", "dd" => "4"];
echo "strtr: ", strtr("aabbccdd", $d), "\n";

$e = ["aa" => new UnsetPats];
$d = &$e;
echo "strtr single: ", strtr("aabb", $e), "\n";

class UnsetSearch implements Stringable {
    public function __toString(): string {
        global $f;
        $f = null;
        return "a";
    }
}

$f = [new UnsetSearch, "b", "c", "d"];
echo "str_replace search: ", str_replace($f, "z", "abcd"), "\n";

class UnsetReplace implements Stringable {
    public function __toString(): string {
        global $g;
        $g = null;
        return "z";
    }
}

$g = [new UnsetReplace, "y", "y", "y"];
echo "str_replace replace: ", str_replace(["a", "b", "c", "d"], $g, "abcd"), "\n";

class UnsetSubject implements Stringable {
    public function __toString(): string {
        global $h;
        $h = null;
        return "abcd";
    }
}

$h = [new UnsetSubject, "abcd"];
var_dump(str_replace("a", "z", $h));
?>
--EXPECT--
destroyed: X,2,3,4
NULL
appended: X,2,3,4
count: 5
Exception: boom
strtr: X234
strtr single: Xbb
str_replace search: zzzz
str_replace replace: zyyy
array(2) {
  [0]=>
  string(4) "zbcd"
  [1]=>
  string(4) "zbcd"
}
