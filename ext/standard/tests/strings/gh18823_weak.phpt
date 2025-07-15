--TEST--
GH-18823 (setlocale's 2nd and 3rd argument ignores strict_types) - weak mode
--INI--
error_reporting=E_ALL
--FILE--
<?php
class MyStringable {
    public function __toString(): string {
        return 'foo';
    }
}

class MyStringableThrow {
    public function __toString(): string {
        throw new Error('no');
    }
}

setlocale(LC_ALL, 0, "0");
setlocale(LC_ALL, "0", 0);
setlocale(LC_ALL, null);
setlocale(LC_ALL, new MyStringable);

try {
    setlocale(LC_ALL, new MyStringableThrow);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
no
