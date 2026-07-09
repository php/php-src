--TEST--
Markup syntax: the exact plain-PHP lowering, pinned via zend_ast_export (assert)
--EXTENSIONS--
html
--INI--
zend.assertions=1
assert.exception=1
--FILE--
<?php
// A failed assert() exports the compiled expression back to source
// (zend_ast_export). Markup is lowered during parsing, so the message shows
// exactly the plain-PHP form each construct compiles to — with `false &&`
// short-circuiting, nothing is ever evaluated or rendered. This pins the
// documented desugarings (RFC "Desugaring at a glance") without any
// dedicated introspection API.
function desugar(string $expr): string {
    try {
        eval("assert(false && ($expr));");
    } catch (AssertionError $e) {
        return substr($e->getMessage(), strlen("assert(false && "), -1);
    }
    return "NO EXPORT";
}

// A plain element with a literal and an interpolated attribute.
echo desugar('<div class="box" id={$id}>Hello {$name}</div>'), "\n";

// Fragment, component with props + spread, nested element body.
echo desugar('<><Card title="Hi" {...$attrs}><b>f</b>body</Card></>'), "\n";

// A static-method tag resolves its class part like any class name.
echo desugar('<Author::byline name="Ada"/>'), "\n";

// The `:lazy` directive wraps the body in a deferred Html\LazyFragment.
echo desugar('<Auth :lazy>Hello {$name}</Auth>'), "\n";

// Dynamic tags lower to Html\render_dynamic.
echo desugar('<$tag class="x">{$content}</$tag>'), "\n";

// Character references are decoded at compile time, so the exported string
// literal contains the decoded text.
echo desugar('<p>Fish &amp; chips &mdash; &pound;5</p>'), "\n";
?>
--EXPECT--
new \Html\Element('div', ['class' => 'box', 'id' => $id], ['Hello ', $name])
new \Html\Fragment([\Html\render_component(Card::class, ['title' => 'Hi', ...$attrs], new \Html\Fragment([new \Html\Element('b', [], ['f']), 'body']))])
\Html\render_component(Author::class . '::byline', ['name' => 'Ada'], null)
\Html\render_component(Auth::class, [], new \Html\LazyFragment(fn() => new \Html\Fragment(['Hello ', $name])))
\Html\render_dynamic($tag, ['class' => 'x'], [$content])
new \Html\Element('p', [], ['Fish & chips — £5'])
