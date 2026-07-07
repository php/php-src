<?php
/*
 * Render-throughput benchmark: build the same escaped HTML list four ways and
 * compare. Run: sapi/cli/php ext/html/benchmarks/bench_render.php [iterations]
 *
 * The point of comparison:
 *   markup syntax vs plain Html objects -> shows the *syntax* is zero runtime
 *     cost (it compiles to the same `new Html\Element(...)` calls);
 *   Html objects vs string concat/sprintf -> shows the cost of the value-object
 *     tree + escape-by-default vs hand-rolled, hand-escaped strings.
 */

use Html\Element as E;

$ITER = (int) ($argv[1] ?? 50000);

$items = [];
for ($i = 0; $i < 10; $i++) {
    $items[] = ['title' => "Item & $i", 'body' => "Body <$i> text", 'href' => "/item/$i"];
}

function bench(string $name, callable $fn, int $iter): void {
    for ($i = 0; $i < 1000; $i++) { $fn(); }          // warm up
    $t = hrtime(true);
    $out = '';
    for ($i = 0; $i < $iter; $i++) { $out = $fn(); }
    $dt = (hrtime(true) - $t) / 1e9;
    printf("  %-22s %8.3f s   %12s ops/s   (%d bytes)\n",
        $name, $dt, number_format($iter / $dt), strlen($out));
}

// (a) native markup syntax
$markup = static function () use ($items) {
    return (string) <ul class="list">{array_map(fn($it) =>
        <li class="item"><a href={$it['href']}>{$it['title']}</a><p>{$it['body']}</p></li>, $items)}</ul>;
};

// (b) plain Html\Element objects (what the syntax compiles to)
$objects = static function () use ($items) {
    return (string) new E('ul', ['class' => 'list'], [
        array_map(static fn($it) => new E('li', ['class' => 'item'], [
            new E('a', ['href' => $it['href']], [$it['title']]),
            new E('p', [], [$it['body']]),
        ]), $items),
    ]);
};

// (c) manual string concatenation with manual escaping
$concat = static function () use ($items) {
    $h = '<ul class="list">';
    foreach ($items as $it) {
        $h .= '<li class="item"><a href="' . htmlspecialchars($it['href'], ENT_QUOTES, 'UTF-8') . '">'
            . htmlspecialchars($it['title'], ENT_QUOTES, 'UTF-8') . '</a><p>'
            . htmlspecialchars($it['body'], ENT_QUOTES, 'UTF-8') . '</p></li>';
    }
    return $h . '</ul>';
};

// (d) sprintf with manual escaping
$printf = static function () use ($items) {
    $rows = '';
    foreach ($items as $it) {
        $rows .= sprintf('<li class="item"><a href="%s">%s</a><p>%s</p></li>',
            htmlspecialchars($it['href'], ENT_QUOTES, 'UTF-8'),
            htmlspecialchars($it['title'], ENT_QUOTES, 'UTF-8'),
            htmlspecialchars($it['body'], ENT_QUOTES, 'UTF-8'));
    }
    return sprintf('<ul class="list">%s</ul>', $rows);
};

echo "Equivalence check:\n";
printf("  markup === objects : %s\n", $markup() === $objects() ? 'yes' : 'NO');
printf("  markup === concat  : %s\n", $markup() === $concat()  ? 'yes' : 'NO');
printf("  markup === sprintf : %s\n", $markup() === $printf()  ? 'yes' : 'NO');

echo "\nThroughput ($ITER iterations, 10 items each):\n";
bench('markup syntax',      $markup,  $ITER);
bench('plain Html objects', $objects, $ITER);
bench('string concat',      $concat,  $ITER);
bench('sprintf',            $printf,  $ITER);
