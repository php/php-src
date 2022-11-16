--TEST--
setcookie() SameSite parameter errors
--FILE--
<?php

// Check type errors work
try {
    setcookie('name', sameSite: new stdClass());
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

// Check ValueError when using SameSite::None and the cookie is not secure, see
// https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Set-Cookie/SameSite#samesitenone_requires_secure
try {
    setcookie('name', sameSite: SameSite::None);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    setcookie('name', sameSite: SameSite::None, secure: false);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

// Sanity enum check
var_dump(SameSite::Strict);
?>
--EXPECT--
setcookie(): Argument #8 ($sameSite) must be of type SameSite, stdClass given
setcookie(): Argument #8 ($sameSite) can only be SameSite::None if argument #6 ($secure) is true
setcookie(): Argument #8 ($sameSite) can only be SameSite::None if argument #6 ($secure) is true
enum(SameSite::Strict)
