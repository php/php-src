--TEST--
Str hybrid construction: periodic failure paths, partial mismatches and branching boundaries
--EXTENSIONS--
strmatch
--FILE--
<?php
function checkMatcher(array $keywords, string $subject): void {
    $expected = [];
    foreach ($keywords as $keyword) {
        $offset = 0;
        while (($position = strpos($subject, $keyword, $offset)) !== false) {
            $expected[] = [$position, strlen($keyword), $keyword];
            $offset = $position + 1;
        }
    }
    usort($expected, static fn ($a, $b) => [$a[0], $a[1]] <=> [$b[0], $b[1]]);
    $matcher = new Str\Matcher($keywords);
    $actual = array_map(static fn ($hit) => [$hit->offset, $hit->length, $hit->keyword], $matcher->matchAll($subject));
    if ($actual !== $expected
        || $matcher->countMatches($subject) !== count($expected)
        || $matcher->matches($subject) !== (bool) $expected
        || $matcher->getMatchedKeywords($subject) !== array_values(array_unique(array_column($expected, 2)))) {
        var_dump($subject, $expected, $actual);
        throw new Exception('Matcher fallback mismatch');
    }
}

function checkReplacer(array $map, string $subject, string $expected, int $expectedCount): void {
    $replacer = new Str\Replacer($map);
    $actual = $replacer->replace($subject, $count);
    if ($actual !== $expected || $actual !== strtr($subject, $map) || $count !== $expectedCount) {
        var_dump($subject, $expected, $actual, $count);
        throw new Exception('Replacer fallback mismatch');
    }
}

foreach (['abcd', 'abcde', 'abcdef', "\0\x80ab\xff"] as $period) {
    $word = str_repeat($period, 128);
    $prefix = substr($word, 0, -1);
    foreach (['!x' . $word . '?', 'x' . $prefix . 'x' . $word, 'x' . $prefix, 'unrelated'] as $subject) {
        checkMatcher([$word, 'x' . $word], $subject);
    }
    $map = [$word => 'W', $word . 'x' => 'WX'];
    checkReplacer($map, '!' . $word . 'x?', '!WX?', 1);
    checkReplacer($map, $word . 'x' . substr($word, 1) . 'x', 'WX' . substr($word, 1) . 'x', 1);
    checkReplacer($map, substr($word, 1) . 'x', substr($word, 1) . 'x', 0);
}
foreach ([7, 8] as $degree) {
    $map = [];
    for ($i = 0; $i < $degree; $i++) {
        $map['pqrstuABCDE' . chr(128 + $i) . 'tail'] = 'R' . $i;
    }
    $words = array_keys($map);
    $subject = '!' . $words[$degree - 1] . '|' . $words[0] . '?';
    checkMatcher($words, $subject);
    checkReplacer($map, $subject, '!R' . ($degree - 1) . '|R0?', 2);
    $subject = substr($words[0], 0, -1) . $words[$degree - 1];
    checkMatcher($words, $subject);
    checkReplacer($map, $subject, substr($words[0], 0, -1) . 'R' . ($degree - 1), 1);
}
echo "OK\n";
?>
--EXPECT--
OK
