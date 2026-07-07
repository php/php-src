--TEST--
Markup syntax: {...$attrs} spread for elements (array merge) and components (named args) (RFC §5)
--EXTENSIONS--
html
--FILE--
<?php
use Html\Element as E;

// Element: spread merges into the attribute map; values are still escaped.
$extra = ['data-id' => 7, 'class' => 'from-spread', 'title' => 'T & T'];
echo (<a href="/x" {...$extra}>link</a>)->__toString(), "\n";

// Conflict resolution follows array-merge order (last wins).
echo (<a class="base" {...['class' => 'override']}>x</a>)->__toString(), "\n";
echo (<a {...['class' => 'base']} class="explicit">x</a>)->__toString(), "\n";

// Component: spread becomes named arguments (props).
class Card implements Html\Htmlable {
    public function __construct(public string $title, public string $tone = 'plain') {}
    public function toHtml(): Html\Htmlable {
        return new E('div', ['class' => 'card-' . $this->tone], [$this->title]);
    }
}
$props = ['title' => 'Hi & bye', 'tone' => 'loud'];
echo (<Card {...$props}/>)->__toString(), "\n";
echo (<Card title="Explicit" {...['tone' => 'soft']}/>)->__toString(), "\n";

// A component variadic collects unknown (e.g. hyphenated) spread keys.
function Box(string $kind, ...$attrs): Html\Htmlable {
    return new E('div', ['class' => $kind, ...$attrs], ['box']);
}
echo (<Box kind="b" {...['data-x' => '1', 'role' => 'note']}/>)->__toString(), "\n";
?>
--EXPECT--
<a href="/x" data-id="7" class="from-spread" title="T &amp; T">link</a>
<a class="override">x</a>
<a class="explicit">x</a>
<div class="card-loud">Hi &amp; bye</div>
<div class="card-soft">Explicit</div>
<div class="b" data-x="1" role="note">box</div>
