--TEST--
Html\Slot is a bare parameter-target marker attribute (RFC §5)
--EXTENSIONS--
html
--FILE--
<?php
use Html\Slot;

function Layout(
    #[Slot] Html\Htmlable $body,
    ?Html\Htmlable $footer = null,
): Html\Htmlable {
    return new Html\Fragment([$body, $footer]);
}

$ref = new ReflectionFunction('Layout');
foreach ($ref->getParameters() as $param) {
    foreach ($param->getAttributes(Slot::class) as $attr) {
        $slot = $attr->newInstance();
        printf("%s -> %s\n", $param->getName(), get_class($slot));
    }
}

// Slot takes no arguments and only targets parameters.
$attr = (new ReflectionClass(Slot::class))->getAttributes(Attribute::class)[0]->newInstance();
var_dump($attr->flags === Attribute::TARGET_PARAMETER);
?>
--EXPECT--
body -> Html\Slot
bool(true)
