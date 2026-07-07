--TEST--
Markup syntax: dynamic tag error cases (mismatched close, slots on elements, invalid names)
--EXTENSIONS--
html
--FILE--
<?php
// Mismatched closing variable is a compile error, like </div> for <span>.
try {
    eval('$x = <$a>hi</$b>;');
} catch (CompileError $e) {
    echo $e->getMessage(), "\n";
}

// A brace tag closes anonymously; a named or variable close is a parse error
// (the expression cannot be restated without re-evaluating it).
foreach (['$x = <{"div"}>a</div>;', '$x = <{"div"}>a</$t>;'] as $code) {
    try {
        eval($code);
    } catch (ParseError $e) {
        echo $e->getMessage(), "\n";
    }
}

// A <slot:name> block against an element-classified value has nowhere to go.
$tag = 'div';
try {
    $x = <$tag><slot:footer>x</slot:footer></$tag>;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

// An empty tag value is rejected up front.
try {
    Html\render_dynamic('');
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

// A value that is no valid tag name fails at serialization (the existing
// Element guard) - a dynamic name can never break out of the markup.
$tag = 'di v';
try {
    echo (string) <$tag/>;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

// A lowercase value is always an element, even if a function of that name
// exists (the same rule that makes static <date> an element).
$tag = 'date';
echo <$tag/>, "\n";

// A capitalized value resolving to an internal function hits the footgun guard.
$tag = 'Date';
try {
    echo <$tag/>;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

// A capitalized value with no symbol behind it.
$tag = 'NoSuchComponent';
try {
    echo <$tag/>;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Mismatched markup closing tag: expected </$a>, found </$b>
syntax error, unexpected markup tag name "div", expecting markup tag end
syntax error, unexpected variable "$t", expecting markup tag end
Markup slot blocks are only allowed in a component body; "div" is an HTML element
Html\render_dynamic(): Argument #1 ($tag) cannot be empty
Invalid tag name "di v"
<date></date>
<Date> resolved to the internal function Date(), which cannot be a component
"NoSuchComponent" is not a component: no class implementing Html\Htmlable and no user-defined function of that name
