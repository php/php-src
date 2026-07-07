--TEST--
Markup syntax: multi-line elements report errors on the opening-tag line, like any multi-line call
--EXTENSIONS--
html
--FILE--
<?php
class Card implements Html\Htmlable {
    public function __construct(private string $title) {}
    public function toHtml(): Html\Htmlable { return <div>{$this->title}</div>; }
}

// Static component spanning lines: the render_component call site is the
// opening <Card line, not the closing </Card> line.
try {
    echo <Card
        title={[]}
    >
    </Card>;
} catch (TypeError $e) {
    /* trace: [0] the internal ctor frame (no line), [1] the render_component
     * call site - which must be the opening-tag line. */
    echo "component call site: line ", $e->getTrace()[1]['line'], "\n";
}

// Dynamic tag spanning lines: same rule.
$var = new stdClass;
try {
    echo <$var>
        body
    </$var>;
} catch (TypeError $e) {
    echo "dynamic tag: line ", $e->getLine(), "\n";
}
?>
--EXPECT--
component call site: line 10
dynamic tag: line 23
