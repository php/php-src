--TEST--
Html\Element attribute value coercion (RFC §5)
--EXTENSIONS--
html
--FILE--
<?php
use Html\Element;

// null and false omit the attribute; true is a bare boolean attribute.
echo (new Element('input', [
    'type' => 'checkbox',
    'checked' => true,
    'disabled' => false,
    'value' => null,
    'tabindex' => 3,
    'data-ratio' => 1.5,
]))->__toString(), "\n";

// Stringable attribute value.
$s = new class implements Stringable {
    public function __toString(): string { return 'x&y'; }
};
echo (new Element('a', ['data-x' => $s]))->__toString(), "\n";

// Arrays are reserved -> TypeError.
try {
    (new Element('div', ['class' => ['a', 'b']]))->__toString();
} catch (\Throwable $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
<input type="checkbox" checked tabindex="3" data-ratio="1.5">
<a data-x="x&amp;y"></a>
TypeError: Attribute "class" value must be of type string|int|float|bool|null|Stringable, array given
