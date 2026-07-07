--TEST--
Html\Slot attribute is reflectable with optional name (RFC §6)
--EXTENSIONS--
html
--FILE--
<?php
use Html\Slot;

function Layout(
    #[Slot] Html\Htmlable $body,
    #[Slot('footer')] ?Html\Htmlable $footer = null,
): Html\Htmlable {
    return new Html\Fragment([$body, $footer]);
}

$ref = new ReflectionFunction('Layout');
foreach ($ref->getParameters() as $param) {
    foreach ($param->getAttributes(Slot::class) as $attr) {
        $slot = $attr->newInstance();
        printf("%s -> name=%s\n", $param->getName(), var_export($slot->name, true));
    }
}

// Slot only targets parameters.
$attr = (new ReflectionClass(Slot::class))->getAttributes(Attribute::class)[0]->newInstance();
var_dump($attr->flags === Attribute::TARGET_PARAMETER);
?>
--EXPECT--
body -> name=NULL
footer -> name='footer'
bool(true)
