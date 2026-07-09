--TEST--
Markup syntax: hyphenated attributes on components route through named args (RFC §5)
--EXTENSIONS--
html
--FILE--
<?php
use Html\Element as E;

// A component that collects arbitrary (incl. hyphenated) attributes via a variadic.
class Box implements Html\Htmlable {
    private array $attrs;
    public function __construct(public string $kind, ...$attrs) { $this->attrs = $attrs; }
    public function toHtml(): Html\Htmlable {
        return new E('div', ['class' => $this->kind, ...$this->attrs], ['box']);
    }
}

// Hyphenated attributes work directly on a component (no spread needed) - they
// become named arguments and the variadic collects them.
echo (<Box kind="note" data-id="7" aria-label="hi"/>), "\n";

// A component without a variadic rejects an unknown (hyphenated) attribute.
class Tight implements Html\Htmlable {
    public function __construct(public string $kind) {}
    public function toHtml(): Html\Htmlable { return new E('div', ['class' => $this->kind], ['y']); }
}
try {
    echo (<Tight kind="b" data-x="1"/>), "\n";
} catch (\Throwable $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
<div class="note" data-id="7" aria-label="hi">box</div>
Error: Unknown named parameter $data-x
