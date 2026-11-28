--TEST--
Str\Matcher and Str\Replacer compare their normalized dictionaries
--FILE--
<?php
function compare(string $label, object $left, object $right, bool $equal): void {
    if (($left == $right) !== $equal
        || ($right == $left) !== $equal
        || ($left != $right) !== !$equal
        || ($right != $left) !== !$equal) {
        throw new Exception($label);
    }
    echo $label, ': ', $equal ? 'equal' : 'different', "\n";
}

compare('Matcher same dictionary', new Str\Matcher(['a', 'b']), new Str\Matcher(['a', 'b']), true);
compare('Matcher ignores input keys and duplicates', new Str\Matcher(['key' => 'a', 7 => 'b', 'a']), new Str\Matcher(['a', 'b']), true);
compare('Matcher ignores order', new Str\Matcher(['a', 'b']), new Str\Matcher(['b', 'a']), true);
compare('Matcher different key', new Str\Matcher(['a']), new Str\Matcher(['b']), false);
compare('Matcher different size', new Str\Matcher(['a']), new Str\Matcher(['a', 'b']), false);
compare('Matcher numeric strings', new Str\Matcher(['01']), new Str\Matcher(['1']), false);
compare('Matcher binary', new Str\Matcher(["a\0b"]), new Str\Matcher(["a\0b"]), true);
compare('Matcher binary difference', new Str\Matcher(["a\0b"]), new Str\Matcher(["a\0c"]), false);

compare('Replacer same mapping', new Str\Replacer(['a' => 'A', 'b' => 'B']), new Str\Replacer(['a' => 'A', 'b' => 'B']), true);
compare('Replacer ignores order', new Str\Replacer(['a' => 'A', 'b' => 'B']), new Str\Replacer(['b' => 'B', 'a' => 'A']), true);
compare('Replacer reordered different value', new Str\Replacer(['a' => 'A', 'b' => 'B']), new Str\Replacer(['b' => 'B', 'a' => 'C']), false);
compare('Replacer different value', new Str\Replacer(['a' => 'A']), new Str\Replacer(['a' => 'B']), false);
compare('Replacer different key', new Str\Replacer(['a' => 'A']), new Str\Replacer(['b' => 'A']), false);
compare('Replacer different size', new Str\Replacer(['a' => 'A']), new Str\Replacer(['a' => 'A', 'b' => 'B']), false);
compare('Replacer numeric keys', new Str\Replacer([12 => 'A', 'b' => 'B']), new Str\Replacer(['b' => 'B', '12' => 'A']), true);
compare('Replacer distinct numeric keys', new Str\Replacer(['01' => 'A']), new Str\Replacer(['1' => 'A']), false);
compare('Replacer numeric values', new Str\Replacer(['a' => '01']), new Str\Replacer(['a' => '1']), false);
compare('Replacer empty values', new Str\Replacer(['a' => '', 'b' => 'B']), new Str\Replacer(['b' => 'B', 'a' => '']), true);
compare('Replacer reordered numeric values', new Str\Replacer(['a' => '01', 'b' => 'B']), new Str\Replacer(['b' => 'B', 'a' => '1']), false);
compare('Replacer binary', new Str\Replacer(["a\0" => "A\0", 'b' => 'B']), new Str\Replacer(['b' => 'B', "a\0" => "A\0"]), true);
compare('Replacer binary value difference', new Str\Replacer(["a\0" => "A\0", 'b' => 'B']), new Str\Replacer(['b' => 'B', "a\0" => "A\1"]), false);
compare('Replacer binary key difference', new Str\Replacer(["a\0" => 'A', 'b' => 'B']), new Str\Replacer(['b' => 'B', "a\1" => 'A']), false);

foreach ([Str\Matcher::class, Str\Replacer::class] as $class) {
    compare($class . ' empty', new $class([]), new $class([]), true);
    $dictionary = $class === Str\Matcher::class ? ['a'] : ['a' => 'A'];
    $first = new $class($dictionary);
    $second = new $class($dictionary);
    compare($class . ' self', $first, $first, true);
    var_dump($first === $first, $first === $second);
    var_dump(in_array($second, [$first]), in_array($second, [$first], true));
    var_dump(array_search($second, [$first]), array_search($second, [$first], true));
    compare($class . ' other class', $first, new stdClass(), false);
}
compare('Different prepared classes', new Str\Matcher(['a']), new Str\Replacer(['a' => 'a']), false);
compare('Different empty classes', new Str\Matcher([]), new Str\Replacer([]), false);

$keyword = 'a';
$matcher = new Str\Matcher([&$keyword]);
$keyword = 'b';
$matcher->matchAll('aaaa');
compare('Matcher snapshot after use', $matcher, new Str\Matcher(['a']), true);
$value = 'A';
$replacer = new Str\Replacer(['a' => &$value, 'b' => 'B']);
$value = 'C';
$replacer->replace('aba');
compare('Replacer snapshot after use', $replacer, new Str\Replacer(['b' => 'B', 'a' => 'A']), true);

$first = (new Str\Matcher(['a']))->matchAll('a')[0];
$second = (new Str\Matcher(['a']))->matchAll('a')[0];
compare('MatchResult', $first, $second, true);
?>
--EXPECT--
Matcher same dictionary: equal
Matcher ignores input keys and duplicates: equal
Matcher ignores order: equal
Matcher different key: different
Matcher different size: different
Matcher numeric strings: different
Matcher binary: equal
Matcher binary difference: different
Replacer same mapping: equal
Replacer ignores order: equal
Replacer reordered different value: different
Replacer different value: different
Replacer different key: different
Replacer different size: different
Replacer numeric keys: equal
Replacer distinct numeric keys: different
Replacer numeric values: different
Replacer empty values: equal
Replacer reordered numeric values: different
Replacer binary: equal
Replacer binary value difference: different
Replacer binary key difference: different
Str\Matcher empty: equal
Str\Matcher self: equal
bool(true)
bool(false)
bool(true)
bool(false)
int(0)
bool(false)
Str\Matcher other class: different
Str\Replacer empty: equal
Str\Replacer self: equal
bool(true)
bool(false)
bool(true)
bool(false)
int(0)
bool(false)
Str\Replacer other class: different
Different prepared classes: different
Different empty classes: different
Matcher snapshot after use: equal
Replacer snapshot after use: equal
MatchResult: equal
