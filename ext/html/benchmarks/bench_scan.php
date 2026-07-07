<?php
/*
 * Scanner-overhead benchmark: tokenize a large *non-markup* PHP source many
 * times via token_get_all(). Used to A/B the cost of the markup scanner's
 * `last_token` tracking on ordinary PHP code (build the engine with and without
 * the two `SCNG(last_token) = token;` stores and compare).
 *
 * The source deliberately avoids "<>" and "<tag" so the token stream is
 * identical with and without the tracking - only the per-token store differs.
 *
 * Run: sapi/cli/php ext/html/benchmarks/bench_scan.php [iterations]
 */

$ITER = (int) ($argv[1] ?? 3000);

$src = "<?php\n";
for ($i = 0; $i < 2000; $i++) {
    $src .= "\$a$i = \$b < \$c && \$d <= \$e ? foo(\$x, \$y) : bar(\$z);\n";
    $src .= "if (\$a$i >= 10) { echo \"line $i is \$a$i\"; }\n";
}
$len = strlen($src);

for ($i = 0; $i < 10; $i++) { token_get_all($src); }   // warm up

$t = hrtime(true);
$count = 0;
for ($i = 0; $i < $ITER; $i++) { $count = count(token_get_all($src)); }
$dt = (hrtime(true) - $t) / 1e9;

printf("source: %d bytes, %d tokens/pass\n", $len, $count);
printf("token_get_all x %d: %.3f s   %.1f MB/s   %s tokens/s\n",
    $ITER, $dt, ($len * $ITER) / 1e6 / $dt, number_format($count * $ITER / $dt));
