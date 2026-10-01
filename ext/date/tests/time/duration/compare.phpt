--TEST--
Time\Duration: Comparison handlers
--FILE--
<?php

require __DIR__ . '/helper.inc';

$durations = [
    Time\Duration::fromSeconds(0, 0),
    Time\Duration::fromSeconds(0, 1),
    Time\Duration::fromSeconds(0, 2),
    Time\Duration::fromSeconds(1, 0),
    Time\Duration::fromSeconds(1, 1),
    Time\Duration::fromSeconds(1, 2),
    Time\Duration::fromSeconds(2, 0),
    Time\Duration::fromSeconds(2, 1),
    Time\Duration::fromSeconds(2, 2),
];

function n(int $result) {
    return ($result === 0 ? '=' : ($result < 0 ? '<' : '>'));
}

$durations = [
    ...$durations,
    null,
    ...negate_all($durations),
];

foreach ($durations as $a) {
    if ($a === null) {
        continue;
    }

    foreach ($durations as $b) {
        if ($b === null) {
            continue;
        }

        if (n($a <=> $b) !== n(Time\Duration::compare($a, $b))) {
            echo sprintf('%1$s <=> %2$s !== Duration::compare(%1$s, %2$s)', f($a, pad: false), f($b, pad: false)), PHP_EOL;
        }
    }
}

?>
==DONE==
--EXPECT--
==DONE==
