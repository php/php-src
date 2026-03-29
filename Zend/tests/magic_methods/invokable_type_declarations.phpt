--TEST--
Invokable in various type declaration contexts
--FILE--
<?php

class Handler {
    public function __invoke(string $s): string {
        return strtoupper($s);
    }
}

/* Parameter type */
function acceptInvokable(Invokable $fn): void {
    echo $fn("hello") . "\n";
}

acceptInvokable(new Handler());
acceptInvokable(fn(string $s): string => strtolower($s));

/* Return type */
function getInvokable(): Invokable {
    return new Handler();
}
var_dump(getInvokable() instanceof Invokable);

/* Nullable */
function maybeInvokable(?Invokable $fn): void {
    if ($fn !== null) {
        echo $fn("world") . "\n";
    } else {
        echo "null\n";
    }
}
maybeInvokable(new Handler());
maybeInvokable(null);

/* Union type */
function invokableOrString(Invokable|string $val): void {
    if ($val instanceof Invokable) {
        echo $val("union") . "\n";
    } else {
        echo $val . "\n";
    }
}
invokableOrString(new Handler());
invokableOrString("plain string");

/* instanceof checks: negative */
var_dump(new stdClass() instanceof Invokable);
var_dump("strlen" instanceof Invokable);

?>
--EXPECT--
HELLO
hello
bool(true)
WORLD
null
UNION
plain string
bool(false)
bool(false)
