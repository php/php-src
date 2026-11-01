--TEST--
Str methods agree with independent occurrence and replacement oracles (350 seeded cases)
--EXTENSIONS--
strmatch
--FILE--
<?php
function randomWord(int $maxLength, int $alphabetSize): string {
    $length = mt_rand(1, $maxLength);
    $word = '';
    for ($i = 0; $i < $length; $i++) {
        $word .= chr(mt_rand(97, 96 + $alphabetSize));
    }
    return $word;
}

function checkCase(array $map, string $subject): void {
    $keywords = array_keys($map);
    $hits = [];
    foreach ($keywords as $keyword) {
        $offset = 0;
        while (($position = strpos($subject, $keyword, $offset)) !== false) {
            $hits[] = [$position, strlen($keyword), $keyword];
            $offset = $position + 1;
        }
    }
    usort($hits, static fn ($a, $b) => [$a[0], $a[1]] <=> [$b[0], $b[1]]);
    $unique = array_values(array_unique(array_column($hits, 2)));
    $matcher = new Str\Matcher($keywords);
    $actual = array_map(static fn ($hit) => [$hit->offset, $hit->length, $hit->keyword], $matcher->matchAll($subject));
    if ($actual !== $hits
        || $matcher->matches($subject) !== (bool) $hits
        || $matcher->countMatches($subject) !== count($hits)
        || $matcher->getMatchedKeywords($subject) !== $unique
        || $matcher->getKeywords() !== $keywords) {
        var_dump($map, $subject, $hits, $actual);
        throw new Exception('Matcher oracle mismatch');
    }

    usort($hits, static fn ($a, $b) => ($a[0] <=> $b[0]) ?: ($b[1] <=> $a[1]));
    $output = '';
    $end = 0;
    $count = 0;
    foreach ($hits as [$offset, $length, $keyword]) {
        if ($offset < $end) {
            continue;
        }
        $output .= substr($subject, $end, $offset - $end) . $map[$keyword];
        $end = $offset + $length;
        $count++;
    }
    $output .= substr($subject, $end);
    $replacer = new Str\Replacer($map);
    $actual = $replacer->replace($subject, $actualCount);
    if ($actual !== $output || $actualCount !== $count
        || $actual !== strtr($subject, $map) || $replacer->getReplacements() !== $map) {
        var_dump($map, $subject, $output, $actual, $count, $actualCount);
        throw new Exception('Replacer oracle mismatch');
    }
}

mt_srand(20261028);
for ($case = 0; $case < 200; $case++) {
    $map = [];
    $size = mt_rand(1, 12);
    for ($i = 0; $i < $size; $i++) {
        $map[randomWord(6, 3)] = strtoupper(randomWord(6, 3));
    }
    $subject = '';
    $length = mt_rand(0, 40);
    for ($i = 0; $i < $length; $i++) {
        $subject .= chr(mt_rand(97, 99));
    }
    checkCase($map, $subject);
}

mt_srand(424242);
for ($case = 0; $case < 150; $case++) {
    $keys = [];
    $size = mt_rand(1, 10);
    for ($i = 0; $i < $size; $i++) {
        $keys[randomWord(5, 4)] = true;
    }
    $subject = '';
    $length = mt_rand(0, 30);
    for ($i = 0; $i < $length; $i++) {
        $subject .= chr(mt_rand(97, 100));
    }
    checkCase(array_fill_keys(array_keys($keys), 'R'), $subject);
}
echo "350 cases OK\n";
?>
--EXPECT--
350 cases OK
