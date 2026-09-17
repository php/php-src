--TEST--
GH-23722 (Use-after-free when __toString() destroys the in_array() haystack)
--FILE--
<?php
class UnsetHaystack implements Stringable {
    public function __toString(): string {
        global $a;
        $a = null;
        return "X";
    }
}

class AppendHaystack implements Stringable {
    public function __toString(): string {
        global $a;
        $a[] = "X";
        return "X";
    }
}

class Boom implements Stringable {
    public function __toString(): string {
        global $a;
        $a = null;
        throw new Exception("boom");
    }
}

/* Built from a variable: an all-constant array literal is kept alive by the
   op_array literal table, so __toString() could not drop it to zero. */
function haystack(): array {
    $first = "b";
    return [$first, "c", "d"];
}

function destroyed(): void {
    global $a;
    $a = haystack();
    echo "destroyed: ";
    var_dump(in_array(new UnsetHaystack, $a));
    var_dump($a);
}

function destroyedWithStrictArg(): void {
    global $a;
    $a = haystack();
    echo "destroyed, with strict argument: ";
    var_dump(in_array(new UnsetHaystack, $a, false));
    var_dump($a);
}

function appended(): void {
    global $a;
    $a = haystack();
    echo "appended: ";
    var_dump(in_array(new AppendHaystack, $a));
    echo "count: ", count($a), "\n";
}

function threw(): void {
    global $a;
    $a = haystack();
    try {
        var_dump(in_array(new Boom, $a));
    } catch (Exception $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    var_dump($a);
}

destroyed();
destroyedWithStrictArg();
appended();
threw();
?>
--EXPECT--
destroyed: bool(false)
NULL
destroyed, with strict argument: bool(false)
NULL
appended: bool(false)
count: 6
Exception: boom
NULL
