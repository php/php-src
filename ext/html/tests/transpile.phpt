--TEST--
Html\transpile() exports the plain-PHP lowering of markup expressions
--EXTENSIONS--
html
--FILE--
<?php
// A plain element with a literal and an interpolated attribute.
echo Html\transpile(<<<'CODE'
<?php
echo <div class="box" id={$id}>Hello {$name}</div>;
CODE);

// Fragment, component with props + spread, named slot, nested element.
echo Html\transpile(<<<'CODE'
<?php
$page = <><Card title="Hi" {...$attrs}><slot:footer><b>f</b></slot:footer>body</Card></>;
CODE);

// Character references are decoded at compile time, so the exported string
// literal contains the decoded text.
echo Html\transpile(<<<'CODE'
<?php
echo <p>Fish &amp; chips &mdash; &pound;5</p>;
CODE);

// Non-markup code passes through as ordinary exported source.
echo Html\transpile(<<<'CODE'
<?php
function double(int $x): int {
	return $x * 2;
}
CODE);

// Invalid code throws ParseError.
try {
    Html\transpile('<?php echo <div>;');
} catch (ParseError $e) {
    echo "ParseError: ", $e->getMessage(), "\n";
}
?>
--EXPECT--
echo new \Html\Element('div', ['class' => 'box', 'id' => $id], ['Hello ', $name]);
$page = new \Html\Fragment([\Html\render_component(Card::class, ['title' => 'Hi', ...$attrs], new \Html\Fragment(['body']), ['footer' => new \Html\Fragment([new \Html\Element('b', [], ['f'])])], 'Card')]);
echo new \Html\Element('p', [], ['Fish & chips — £5']);
function double(int $x): int {
    return $x * 2;
}

ParseError: syntax error, unexpected end of file
