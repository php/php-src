--TEST--
Markup syntax: `#` attributes are the directive namespace; only `#lazy` on a component is valid
--EXTENSIONS--
markup
--FILE--
<?php

function check(string $code): void {
    try {
        eval($code);
        echo "ok\n";
    } catch (CompileError $e) {
        echo $e->getMessage(), PHP_EOL;
    }
}

class Card implements Markup\Html {
    public function __construct(#[Markup\Slot] public ?Markup\Html $slot = null) {}
    public function toHtml(): Markup\Html { return <div>{$this->slot}</div>; }
}

// `#lazy` on a component tag is the one valid directive.
check('echo <Card #lazy>body</Card>;');

// Any other `#` attribute is an unknown directive, on components and elements alike.
check('echo <Card #defer>body</Card>;');
check('echo <div #defer>body</div>;');

// `#lazy` outside a component tag is a compile error, not a literal attribute.
check('echo <div #lazy>body</div>;');
check('$t = "div"; echo <$t #lazy>body</$t>;');
check('$t = "div"; echo <$t #lazy />;');
check('echo <{"div"} #lazy>body</>;');
check('echo <{"div"} #lazy />;');

?>
--EXPECT--
<div>body</div>ok
Unknown markup directive "#defer"
Unknown markup directive "#defer"
The #lazy directive is only valid on a component tag
The #lazy directive is only valid on a component tag
The #lazy directive is only valid on a component tag
The #lazy directive is only valid on a component tag
The #lazy directive is only valid on a component tag
