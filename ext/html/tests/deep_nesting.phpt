--TEST--
Html: pathologically deep child nesting is bounded, not a stack overflow
--EXTENSIONS--
html
--FILE--
<?php
use Html\Element as E;

// A cheap-to-build, deeply nested array must throw, not crash the process.
$kids = ['x'];
for ($i = 0; $i < 100000; $i++) {
    $kids = [$kids];
}
try {
    echo (new E('div', [], $kids))->__toString(), "\n";
} catch (\Throwable $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}

// Normal (shallow) nesting still renders.
echo (new E('ul', [], [[[new E('li', [], ['ok'])]]]))->__toString(), "\n";
?>
--EXPECT--
Error: Maximum markup nesting depth of 1024 exceeded
<ul><li>ok</li></ul>
