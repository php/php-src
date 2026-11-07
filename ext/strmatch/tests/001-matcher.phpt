--TEST--
Str\Matcher: methods, start order, empty inputs and dictionary deduplication
--EXTENSIONS--
strmatch
--FILE--
<?php
$m = new Str\Matcher(['he', 'she', 'his', 'hers']);
var_dump($m->matches('ushers'), $m->matches('nothing'));
var_dump($m->countMatches('ushers'));
print_r($m->getMatchedKeywords('ushers'));
foreach ($m->matchAll('ushers') as $r) {
    printf("%d:%d:%s\n", $r->offset, $r->length, $r->keyword);
}
print_r($m->getKeywords());

// Start order differs from the order in which matches end.
print_r((new Str\Matcher(['abcd', 'bc']))->getMatchedKeywords('abcd'));
foreach ([[], ['word']] as $dictionary) {
    $matcher = new Str\Matcher($dictionary);
    $subjects = $dictionary === [] ? ['', 'word'] : [''];
    foreach ($subjects as $subject) {
        $actual = [
            $matcher->matches($subject),
            $matcher->countMatches($subject),
            $matcher->getMatchedKeywords($subject),
            $matcher->matchAll($subject),
            $matcher->getKeywords(),
        ];
        if ($actual !== [false, 0, [], [], $dictionary]) {
            var_dump($actual);
            throw new Exception('Empty input mismatch');
        }
    }
}
echo "Empty inputs OK\n";
$matcher = new Str\Matcher(['b', 'a', 'b', 'c', 'a']);
$hits = array_map(static fn ($hit) => [$hit->offset, $hit->length, $hit->keyword], $matcher->matchAll('abca'));
if ($matcher->getKeywords() !== ['b', 'a', 'c']
    || $matcher->getMatchedKeywords('abca') !== ['a', 'b', 'c']
    || $matcher->countMatches('abca') !== 4
    || !$matcher->matches('abca')
    || $hits !== [[0, 1, 'a'], [1, 1, 'b'], [2, 1, 'c'], [3, 1, 'a']]) {
    throw new Exception('Deduplication mismatch');
}
echo "Deduplication OK\n";
?>
--EXPECT--
bool(true)
bool(false)
int(3)
Array
(
    [0] => she
    [1] => he
    [2] => hers
)
1:3:she
2:2:he
2:4:hers
Array
(
    [0] => he
    [1] => she
    [2] => his
    [3] => hers
)
Array
(
    [0] => abcd
    [1] => bc
)
Empty inputs OK
Deduplication OK
