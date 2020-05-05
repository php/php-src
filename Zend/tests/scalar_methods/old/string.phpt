--TEST--
Testing string handler methods
--FILE--
<?php

use extension string str\Handler;

error_reporting(E_ALL);

require __DIR__ . '/bootstrap.inc';
require __DIR__ . '/../handlers/string.inc';

str('foobar');

r('length()');

sep();
r('slice(1)');
r('slice(1, null)');
r('slice(-1)');
r('slice(-1, null)');
r('slice(1, 4)');
r('slice(1, -1)');
r('slice(-5, -1)');
r('slice(6)');
r('slice(0, -6)');
r('slice(3, 0)');
r('slice(3, -3)');
r('slice(7)');
r('slice(-7)');
r('slice(0, -7)');
r('slice(3, -4)');
r('slice(0, 7)');
r('slice(3, 4)');

sep();
r('replaceSlice("raboo", 1)');
r('replaceSlice("raboo", 1, null)');
r('replaceSlice("R", -1)');
r('replaceSlice("R", -1, null)');
r('replaceSlice("aboo", 1, 4)');
r('replaceSlice("aboo", 1, -1)');
r('replaceSlice("aboo", -5, -1)');
r('replaceSlice("baz", 6)');
r('replaceSlice("baz", 0, -6)');
r('replaceSlice("baz", 3, 0)');
r('replaceSlice("baz", 3, -3)');
r('replaceSlice("", 1, 4)');
r('replaceSlice("nop", 7)');
r('replaceSlice("nop", -7)');
r('replaceSlice("nop", 0, -7)');
r('replaceSlice("nop", 3, -4)');
r('replaceSlice("nop", 0, 7)');
r('replaceSlice("nop", 3, 4)');

str('abc def abc def abc');

r('indexOf("abc")');
r('indexOf("cab")');
r('indexOf("abc", 1)');
r('indexOf("abc", 8)');
r('indexOf("abc", -19)');
r('indexOf("abc", -18)');
r('indexOf("abc", -11)');
r('indexOf("def", -3)');
r('indexOf("")');
r('indexOf("", 8)');
r('indexOf("", -11)');
r('indexOf("abc", -20)');
r('indexOf("abc", 20)');

sep();
r('lastIndexOf("abc")');
r('lastIndexOf("cab")');
r('lastIndexOf("abc", null)');
r('lastIndexOf("cab", null)');
r('lastIndexOf("abc", 15)');
r('lastIndexOf("abc", 8)');
r('lastIndexOf("abc", -3)');
r('lastIndexOf("abc", -4)');
r('lastIndexOf("abc", -11)');
r('lastIndexOf("def", 3)');
r('lastIndexOf("")');
r('lastIndexOf("", 8)');
r('lastIndexOf("", -11)');
r('lastIndexOf("abc", -20)');
r('lastIndexOf("abc", 20)');

sep();
r('contains("abc")');
r('contains("cab")');
r('contains("")');
r('startsWith("abc")');
r('startsWith("cab")');
r('startsWith("")');
r('endsWith("abc")');
r('endsWith("cab")');
r('endsWith("")');

sep();
r('count("abc")');
r('count("cab")');
r('count("abc", 1)');
r('count("abc", 8)');
r('count("abc", 1, null)');
r('count("abc", 8, null)');
r('count("abc", 8, 5)');
r('count("abc", 8, -3)');
r('count("abc", -19)');
r('count("abc", -18)');
r('count("abc", -11)');
r('count("abc", -11, 5)');
r('count("abc", -11, -3)');
r('count("def", -3)');
r('count("")');
r('count("", 8)');
r('count("", -11)');
r('count("nop", -20)');
r('count("nop", 20)');
r('count("nop", 0, -20)');
r('count("nop", 10, -11)');
r('count("nop", 0, 20)');
r('count("nop", 10, 11)');

str('ooooooooooo');

r('count("o")');
r('count("oo")');
r('count("ooo")');
r('count("oooo")');
r('count("ooooo")');

str('foobar');

r('chunk()');
r('chunk(2)');
r('chunk(4)');
r('chunk(0)');
r('chunk(-1)');

sep();
r('repeat(3)');
r('repeat(1)');
r('repeat(0)');
r('repeat(-1)');

str('abcdefabcdefabcdef');

r('replace("abc", "ABC")');
r('replace("abc", "ABC", 1)');
r('replace("abc", "ABC", 2)');
r('replace("abc", "ABC", 3)');
r('replace("abc", "ABC", 4)');
r('replace("abc", "")');
r('replace("abc", "", 2)');
r('replace("ABC", "abc")');
r('replace("ABC", "abc", 1)');

sep();
r('replace([])');
r('replace(["abc" => "ABC"])');
r('replace(["abc" => ""])');
r('replace(["abc" => "", "def" => "DEF"])');
r('replace(["abc" => "ABC", "cde" => "nop"])');
r('replace(["abc" => "ABC", "Cde" => "nop"])');
r('replace(["abc" => "ABC", "def" => "DEF"])');
r('replace(["abc" => "ABC", "ABC" => "nop"])');
r('replace(["abc" => "ABC", "abcdef" => "ABCDEF"])');
r('replace(["abcdef" => "ABCDEF", "abc" => "ABC"])');

sep();
r('replace([], 2)');
r('replace(["abc" => "ABC"], 2)');
r('replace(["abc" => ""], 2)');
r('replace(["abc" => "", "def" => "DEF"], 2)');
r('replace(["abc" => "ABC", "cde" => "nop"], 2)');
r('replace(["abc" => "ABC", "Cde" => "nop"], 2)');
r('replace(["abc" => "ABC", "def" => "DEF"], 2)');
r('replace(["abc" => "ABC", "ABC" => "nop"], 2)');
r('replace(["abc" => "ABC", "abcdef" => "ABCDEF"], 2)');
r('replace(["abcdef" => "ABCDEF", "abc" => "ABC"], 2)');

sep();
r('replace("abc", "ABC", 0)');
r('replace("abc", "ABC", -1)');
r('replace(["abc" => "ABC"], 0)');
r('replace(["abc" => "ABC"], -1)');

r('replace("", "nop")');
r('replace("", "nop", 1)');
r('replace(["abc" => "ABC", "" => "nop"])');
r('replace(["abc" => "ABC", "" => "nop"], 1)');

str('');

r('length()');
r('slice(0)');
r('slice(0, 0)');
r('replaceSlice("foo", 0)');
r('replaceSlice("foo", 0, 0)');
r('indexOf("")');
r('indexOf("", 0)');
r('lastIndexOf("")');
r('lastIndexOf("", 0)');
r('contains("")');
r('startsWith("")');
r('endsWith("")');
r('count("")');
r('count("", 0)');
r('count("", 0, 0)');
r('repeat(3)');
r('chunk(3)');
r('replace("foo", "bar")');
r('replace("foo", "bar", 1)');

str('foobar');

r('length()');

r('reverse()');

r('padLeft(15)');
r('padLeft(15, "123")');
r('padRight(15)');
r('padRight(15, "123")');

str('FoObAr');

r('toLower()');
r('toUpper()');

str('123,456,789');

r('split(",")');
r('split(",", 1)');
r('split(",", 2)');
r('split(",", -1)');

str('     hello     world     ');

r('trim()');
r('trimLeft()');
r('trimRight()');

str('12345hello12345world12345');

r('trim("54321")');
r('trimLeft("54321")');
r('trimRight("54321")');

?>
--EXPECTF--
Working on string "foobar"

length(): int(6)

slice(1): string(5) "oobar"
slice(1, null): string(5) "oobar"
slice(-1): string(1) "r"
slice(-1, null): string(1) "r"
slice(1, 4): string(4) "ooba"
slice(1, -1): string(4) "ooba"
slice(-5, -1): string(4) "ooba"
slice(6): string(0) ""
slice(0, -6): string(0) ""
slice(3, 0): string(0) ""
slice(3, -3): string(0) ""
slice(7):
InvalidArgumentException: Offset must be in range [-len, len]
slice(-7):
InvalidArgumentException: Offset must be in range [-len, len]
slice(0, -7):
InvalidArgumentException: Length too small
slice(3, -4):
InvalidArgumentException: Length too small
slice(0, 7):
InvalidArgumentException: Length too large
slice(3, 4):
InvalidArgumentException: Length too large

replaceSlice("raboo", 1): string(6) "fraboo"
replaceSlice("raboo", 1, null): string(6) "fraboo"
replaceSlice("R", -1): string(6) "foobaR"
replaceSlice("R", -1, null): string(6) "foobaR"
replaceSlice("aboo", 1, 4): string(6) "faboor"
replaceSlice("aboo", 1, -1): string(6) "faboor"
replaceSlice("aboo", -5, -1): string(6) "faboor"
replaceSlice("baz", 6): string(9) "foobarbaz"
replaceSlice("baz", 0, -6): string(9) "bazfoobar"
replaceSlice("baz", 3, 0): string(9) "foobazbar"
replaceSlice("baz", 3, -3): string(9) "foobazbar"
replaceSlice("", 1, 4): string(2) "fr"
replaceSlice("nop", 7):
InvalidArgumentException: Offset must be in range [-len, len]
replaceSlice("nop", -7):
InvalidArgumentException: Offset must be in range [-len, len]
replaceSlice("nop", 0, -7):
InvalidArgumentException: Length too small
replaceSlice("nop", 3, -4):
InvalidArgumentException: Length too small
replaceSlice("nop", 0, 7):
InvalidArgumentException: Length too large
replaceSlice("nop", 3, 4):
InvalidArgumentException: Length too large

Working on string "abc def abc def abc"

indexOf("abc"): int(0)
indexOf("cab"): bool(false)
indexOf("abc", 1): int(8)
indexOf("abc", 8): int(8)
indexOf("abc", -19): int(0)
indexOf("abc", -18): int(8)
indexOf("abc", -11): int(8)
indexOf("def", -3): bool(false)
indexOf(""): int(0)
indexOf("", 8): int(8)
indexOf("", -11): int(8)
indexOf("abc", -20):
InvalidArgumentException: Offset must be in range [-len, len]
indexOf("abc", 20):
InvalidArgumentException: Offset must be in range [-len, len]

lastIndexOf("abc"): int(16)
lastIndexOf("cab"): bool(false)
lastIndexOf("abc", null): int(16)
lastIndexOf("cab", null): bool(false)
lastIndexOf("abc", 15): int(8)
lastIndexOf("abc", 8): int(8)
lastIndexOf("abc", -3): int(16)
lastIndexOf("abc", -4): int(8)
lastIndexOf("abc", -11): int(8)
lastIndexOf("def", 3): bool(false)
lastIndexOf(""): int(19)
lastIndexOf("", 8): int(8)
lastIndexOf("", -11): int(8)
lastIndexOf("abc", -20):
InvalidArgumentException: Offset must be in range [-len, len]
lastIndexOf("abc", 20):
InvalidArgumentException: Offset must be in range [-len, len]

contains("abc"): bool(true)
contains("cab"): bool(false)
contains(""): bool(true)
startsWith("abc"): bool(true)
startsWith("cab"): bool(false)
startsWith(""): bool(true)
endsWith("abc"): bool(true)
endsWith("cab"): bool(false)
endsWith(""): bool(true)

count("abc"): int(3)
count("cab"): int(0)
count("abc", 1): int(2)
count("abc", 8): int(2)
count("abc", 1, null): int(2)
count("abc", 8, null): int(2)
count("abc", 8, 5): int(1)
count("abc", 8, -3): int(1)
count("abc", -19): int(3)
count("abc", -18): int(2)
count("abc", -11): int(2)
count("abc", -11, 5): int(1)
count("abc", -11, -3): int(1)
count("def", -3): int(0)
count(""): int(20)
count("", 8): int(12)
count("", -11): int(12)
count("nop", -20):
InvalidArgumentException: Offset must be in range [-len, len]
count("nop", 20):
InvalidArgumentException: Offset must be in range [-len, len]
count("nop", 0, -20):
InvalidArgumentException: Length too small
count("nop", 10, -11):
InvalidArgumentException: Length too small
count("nop", 0, 20):
InvalidArgumentException: Length too large
count("nop", 10, 11):
InvalidArgumentException: Length too large

Working on string "ooooooooooo"

count("o"): int(11)
count("oo"): int(5)
count("ooo"): int(3)
count("oooo"): int(2)
count("ooooo"): int(2)

Working on string "foobar"

chunk(): array(6) {
  [0]=>
  string(1) "f"
  [1]=>
  string(1) "o"
  [2]=>
  string(1) "o"
  [3]=>
  string(1) "b"
  [4]=>
  string(1) "a"
  [5]=>
  string(1) "r"
}
chunk(2): array(3) {
  [0]=>
  string(2) "fo"
  [1]=>
  string(2) "ob"
  [2]=>
  string(2) "ar"
}
chunk(4): array(2) {
  [0]=>
  string(4) "foob"
  [1]=>
  string(2) "ar"
}
chunk(0):
InvalidArgumentException: Chunk length has to be positive
chunk(-1):
InvalidArgumentException: Chunk length has to be positive

repeat(3): string(18) "foobarfoobarfoobar"
repeat(1): string(6) "foobar"
repeat(0): string(0) ""
repeat(-1):
InvalidArgumentException: Number of repetitions can not be negative

Working on string "abcdefabcdefabcdef"

replace("abc", "ABC"): string(18) "ABCdefABCdefABCdef"
replace("abc", "ABC", 1): string(18) "ABCdefabcdefabcdef"
replace("abc", "ABC", 2): string(18) "ABCdefABCdefabcdef"
replace("abc", "ABC", 3): string(18) "ABCdefABCdefABCdef"
replace("abc", "ABC", 4): string(18) "ABCdefABCdefABCdef"
replace("abc", ""): string(9) "defdefdef"
replace("abc", "", 2): string(12) "defdefabcdef"
replace("ABC", "abc"): string(18) "abcdefabcdefabcdef"
replace("ABC", "abc", 1): string(18) "abcdefabcdefabcdef"

replace([]): string(18) "abcdefabcdefabcdef"
replace(["abc" => "ABC"]): string(18) "ABCdefABCdefABCdef"
replace(["abc" => ""]): string(9) "defdefdef"
replace(["abc" => "", "def" => "DEF"]): string(9) "DEFDEFDEF"
replace(["abc" => "ABC", "cde" => "nop"]): string(18) "ABCdefABCdefABCdef"
replace(["abc" => "ABC", "Cde" => "nop"]): string(18) "ABCdefABCdefABCdef"
replace(["abc" => "ABC", "def" => "DEF"]): string(18) "ABCDEFABCDEFABCDEF"
replace(["abc" => "ABC", "ABC" => "nop"]): string(18) "ABCdefABCdefABCdef"
replace(["abc" => "ABC", "abcdef" => "ABCDEF"]): string(18) "ABCDEFABCDEFABCDEF"
replace(["abcdef" => "ABCDEF", "abc" => "ABC"]): string(18) "ABCDEFABCDEFABCDEF"

replace([], 2): string(18) "abcdefabcdefabcdef"
replace(["abc" => "ABC"], 2): string(18) "ABCdefABCdefabcdef"
replace(["abc" => ""], 2): string(12) "defdefabcdef"
replace(["abc" => "", "def" => "DEF"], 2): string(15) "DEFabcdefabcdef"
replace(["abc" => "ABC", "cde" => "nop"], 2): string(18) "ABCdefABCdefabcdef"
replace(["abc" => "ABC", "Cde" => "nop"], 2): string(18) "ABCdefABCdefabcdef"
replace(["abc" => "ABC", "def" => "DEF"], 2): string(18) "ABCDEFabcdefabcdef"
replace(["abc" => "ABC", "ABC" => "nop"], 2): string(18) "ABCdefABCdefabcdef"
replace(["abc" => "ABC", "abcdef" => "ABCDEF"], 2): string(18) "ABCDEFABCDEFabcdef"
replace(["abcdef" => "ABCDEF", "abc" => "ABC"], 2): string(18) "ABCDEFABCDEFabcdef"

replace("abc", "ABC", 0):
InvalidArgumentException: Limit has to be positive
replace("abc", "ABC", -1):
InvalidArgumentException: Limit has to be positive
replace(["abc" => "ABC"], 0):
InvalidArgumentException: Limit has to be positive
replace(["abc" => "ABC"], -1):
InvalidArgumentException: Limit has to be positive
replace("", "nop"):
InvalidArgumentException: From string can not be an empty string
replace("", "nop", 1):
InvalidArgumentException: From string can not be an empty string
replace(["abc" => "ABC", "" => "nop"]):
InvalidArgumentException: Replacement array keys can not contain an empty string
replace(["abc" => "ABC", "" => "nop"], 1):
InvalidArgumentException: Replacement array keys can not contain an empty string

Working on string ""

length(): int(0)
slice(0): string(0) ""
slice(0, 0): string(0) ""
replaceSlice("foo", 0): string(3) "foo"
replaceSlice("foo", 0, 0): string(3) "foo"
indexOf(""): int(0)
indexOf("", 0): int(0)
lastIndexOf(""): int(0)
lastIndexOf("", 0): int(0)
contains(""): bool(true)
startsWith(""): bool(true)
endsWith(""): bool(true)
count(""): int(1)
count("", 0): int(1)
count("", 0, 0): int(1)
repeat(3): string(0) ""
chunk(3): array(1) {
  [0]=>
  string(0) ""
}
replace("foo", "bar"): string(0) ""
replace("foo", "bar", 1): string(0) ""

Working on string "foobar"

length(): int(6)
reverse(): string(6) "raboof"
padLeft(15): string(15) "         foobar"
padLeft(15, "123"): string(15) "123123123foobar"
padRight(15): string(15) "foobar         "
padRight(15, "123"): string(15) "foobar123123123"

Working on string "FoObAr"

toLower(): string(6) "foobar"
toUpper(): string(6) "FOOBAR"

Working on string "123,456,789"

split(","): array(3) {
  [0]=>
  string(3) "123"
  [1]=>
  string(3) "456"
  [2]=>
  string(3) "789"
}
split(",", 1): array(1) {
  [0]=>
  string(11) "123,456,789"
}
split(",", 2): array(2) {
  [0]=>
  string(3) "123"
  [1]=>
  string(7) "456,789"
}
split(",", -1): array(2) {
  [0]=>
  string(3) "123"
  [1]=>
  string(3) "456"
}

Working on string "     hello     world     "

trim(): string(15) "hello     world"
trimLeft(): string(20) "hello     world     "
trimRight(): string(20) "     hello     world"

Working on string "12345hello12345world12345"

trim("54321"): string(15) "hello12345world"
trimLeft("54321"): string(20) "hello12345world12345"
trimRight("54321"): string(20) "12345hello12345world"
