--TEST--
Markup syntax: <slot:name> blocks route to #[Html\Slot('name')] parameters (RFC §6)
--EXTENSIONS--
html
--FILE--
<?php
use Html\Element as E;
use Html\Slot;
use function Html\raw;

function Layout(
    string $lang,
    #[Slot]            Html\Htmlable $body,
    #[Slot('header')] ?Html\Htmlable $header = null,
    #[Slot('footer')] ?Html\Htmlable $footer = null,
): Html\Htmlable {
    return new E('main', ['lang' => $lang], [
        new E('header', [], [$header]),
        new E('article', [], [$body]),
        new E('footer', [], [$footer]),
    ]);
}

$title = 'Hello & welcome';

// anonymous + named slots together
echo (<Layout lang="en"><slot:header><h1>{$title}</h1></slot:header><p>Loose body.</p><slot:footer>{raw('&copy; 2026')}</slot:footer></Layout>)->__toString(), "\n";

// only anonymous slot
echo (<Layout lang="en"><p>just body</p></Layout>)->__toString(), "\n";

// self-closing named slot (empty)
echo (<Layout lang="en"><slot:header/><p>b</p></Layout>)->__toString(), "\n";

// a <slot:x> with no matching #[Html\Slot('x')] parameter is a runtime error
try {
    echo (<Layout lang="en"><slot:nope>x</slot:nope><p>b</p></Layout>)->__toString(), "\n";
} catch (\Throwable $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
<main lang="en"><header><h1>Hello &amp; welcome</h1></header><article><p>Loose body.</p></article><footer>&copy; 2026</footer></main>
<main lang="en"><header></header><article><p>just body</p></article><footer></footer></main>
<main lang="en"><header></header><article><p>b</p></article><footer></footer></main>
Error: No #[Html\Slot("nope")] parameter accepts the <slot:nope> block
