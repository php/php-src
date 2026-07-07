<?php
/*
 * Component-dispatch overhead: render a list of components via the markup tag
 * (which lowers to Html\render_component) vs constructing the same objects
 * directly. Isolates the cost of runtime name resolution + slot routing +
 * named-argument dispatch. Run: sapi/cli/php ext/html/benchmarks/bench_component.php [iter]
 */

use Html\Element as E;

final class Badge implements Html\Htmlable {
    public function __construct(public string $label, public string $kind = 'info') {}
    public function toHtml(): Html\Htmlable {
        return new E('span', ['class' => "badge badge-{$this->kind}"], [$this->label]);
    }
}

$ITER = (int) ($argv[1] ?? 50000);
$labels = [];
for ($i = 0; $i < 10; $i++) { $labels[] = "L$i"; }

function bench(string $name, callable $fn, int $iter): void {
    for ($i = 0; $i < 1000; $i++) { $fn(); }
    $t = hrtime(true);
    $out = '';
    for ($i = 0; $i < $iter; $i++) { $out = $fn(); }
    $dt = (hrtime(true) - $t) / 1e9;
    printf("  %-26s %8.3f s   %12s ops/s\n", $name, $dt, number_format($iter / $dt));
}

// component tag -> Html\render_component('Badge', ['label'=>..,'kind'=>'info'], null, [])
$viaComponent = static function () use ($labels) {
    return (string) <div>{array_map(static fn($l) => <Badge label={$l} kind="info"/>, $labels)}</div>;
};

// direct construction (no resolution / routing / named-arg dispatch)
$direct = static function () use ($labels) {
    return (string) new E('div', [], [array_map(static fn($l) => new Badge($l, 'info'), $labels)]);
};

printf("equivalent output: %s\n\n", $viaComponent() === $direct() ? 'yes' : 'NO');
echo "Throughput ($ITER iterations, 10 components each):\n";
bench('component tag (dispatch)', $viaComponent, $ITER);
bench('direct construction',      $direct,       $ITER);
