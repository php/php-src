--TEST--
setcookie() and setrawcookie() validate samesite option
--FILE--
<?php
ob_start();

// Valid values
var_dump(setcookie('test', 'value', ['samesite' => 'Strict']));
var_dump(setcookie('test', 'value', ['samesite' => 'Lax']));
var_dump(setcookie('test', 'value', ['samesite' => 'None']));
var_dump(setcookie('test', 'value', ['samesite' => '']));

// Case-insensitive
var_dump(setcookie('test', 'value', ['samesite' => 'strict']));
var_dump(setcookie('test', 'value', ['samesite' => 'LAX']));
var_dump(setcookie('test', 'value', ['samesite' => 'NONE']));

// setrawcookie uses the same validation
var_dump(setrawcookie('test', 'value', ['samesite' => 'Lax']));

// Invalid values
try {
    setcookie('test', 'value', ['samesite' => 'Invalid']);
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    setcookie('test', 'value', ['samesite' => "Strict\r\nX-Injected: evil"]);
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    setrawcookie('test', 'value', ['samesite' => 'Invalid']);
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
setcookie(): "samesite" option must be "Strict", "Lax", "None", or ""
setcookie(): "samesite" option must be "Strict", "Lax", "None", or ""
setrawcookie(): "samesite" option must be "Strict", "Lax", "None", or ""
