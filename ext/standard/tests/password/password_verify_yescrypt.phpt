--TEST--
Test normal operation of password_verify() with Yescrypt
--FILE--
<?php

// TODO: what to do with \0 ???

var_dump(password_verify('letmein', '$y$jA.0./$nE5IrNnOpcaIL7JKLRrJANbOMhoOg/$Gdobttkzk5ona2UNoBZfepu2Gl3w3NQXloEfWz4KSq3'));
var_dump(password_verify('test', '$y$jA.0./$nE5IrNnOpcaIL7JKLRrJANbOMhoOg/$Gdobttkzk5ona2UNoBZfepu2Gl3w3NQXloEfWz4KSq3'));
var_dump(password_verify('letmei', '$y$jA.0./$nE5IrNnOpcaIL7JKLRrJANbOMhoOg/$Gdobttkzk5ona2UNoBZfepu2Gl3w3NQXloEfWz4KSq3'));
var_dump(password_verify('letmein', '$y$jA.1./$nE5IrNnOpcaIL7JKLRrJANbOMhoOg/$Gdobttkzk5ona2UNoBZfepu2Gl3w3NQXloEfWz4KSq3'));
var_dump(password_verify('letmein', '$y$jA.1./$'.chr(0)));

var_dump(password_verify('this is yescrypt', '$y$j9T$oZ1CpcJNi71CnQpOHlY9q.$7KlWq26Kv1pBblGfbg3HQn7j84oZv4dSYIAlZ2YzOh6'));
var_dump(password_verify('test', '$y$j9T$oZ1CpcJNi71CnQpOHlY9q.$7KlWq26Kv1pBblGfbg3HQn7j84oZv4dSYIAlZ2YzOh6'));
var_dump(password_verify('letmein', '$y$j9T$oZ1CpcJNi71CnQpOHlY9q.$7KlWq26Kv1pBblGfbg3HQn7j84oZv4dSYIAlZ2YzOh6'));

echo "OK!";
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
OK!
