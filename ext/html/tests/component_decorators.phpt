--TEST--
Html: component decorators wrap the output of every component kind uniformly (RFC §4)
--EXTENSIONS--
html
--FILE--
<?php
use function Html\register_component_decorator;

class Card implements Html\Htmlable {
    public function __construct(public string $t) {}
    public function toHtml(): Html\Htmlable {
        return new Html\Element('div', [], [$this->t]);
    }
}
function Note(string $t): Html\Htmlable { return new Html\Element('p', [], [$t]); }
class Author {
    public static function byline(string $t): Html\Htmlable {
        return new Html\Element('span', [], [$t]);
    }
}

// A decorator wrapping in a marker that records the component name; proves it
// fires for a class, a function, and a static method alike.
register_component_decorator(fn(Html\Htmlable $h, string $c)
    => Html\raw("[$c]" . $h . "[/$c]"));

echo <Card t="a"/>, "\n";
echo <Note t="b"/>, "\n";
echo <Author::byline t="c"/>, "\n";

// Decorators compose in registration order, each wrapping the previous result.
register_component_decorator(fn(Html\Htmlable $h, string $c) => Html\raw("<<" . $h . ">>"));
echo <Note t="d"/>, "\n";

// A decorator must return an Html\Htmlable.
register_component_decorator(fn(Html\Htmlable $h, string $c) => "not htmlable");
try { echo <Note t="e"/>; }
catch (\Error $e) { echo $e->getMessage(), "\n"; }
?>
--EXPECT--
[Card]<div>a</div>[/Card]
[Note]<p>b</p>[/Note]
[Author::byline]<span>c</span>[/Author::byline]
<<[Note]<p>d</p>[/Note]>>
Component decorator for <Note> did not return an Html\Htmlable
