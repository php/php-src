--TEST--
GH-23725 (Use-after-free when __toString() destroys a str_replace() string argument)
--FILE--
<?php
class UnsetSearch implements Stringable {
    public function __toString(): string {
        global $search;
        $search = null;
        return "z";
    }
}

class UnsetSearchFromSubject implements Stringable {
    public function __toString(): string {
        global $search;
        $search = null;
        return "abcabc";
    }
}

function needle(): string {
    $prefix = "a";
    return $prefix . "bc";
}

function destroyedByReplacement(): void {
    global $search;
    $search = needle();
    echo "search string, freed by replacement: ";
    var_dump(str_replace($search, new UnsetSearch, "abcabc"));
    var_dump($search);
}

function destroyedBySubject(): void {
    global $search;
    $search = needle();
    echo "search string, freed by subject: ";
    var_dump(str_replace($search, "z", new UnsetSearchFromSubject));
    var_dump($search);
}

destroyedByReplacement();
destroyedBySubject();
?>
--EXPECT--
search string, freed by replacement: string(2) "zz"
NULL
search string, freed by subject: string(2) "zz"
NULL
