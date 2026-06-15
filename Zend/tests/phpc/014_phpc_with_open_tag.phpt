--TEST--
.phpc: an accidental literal '<?php' inside a .phpc file is a syntax error (not magic)
--FILE--
<?php
$file = __DIR__ . '/' . basename(__FILE__, '.php') . '.phpc';
/* In .phpc mode the lexer starts in ST_IN_SCRIPTING. A literal '<?php' at
 * the top is now interpreted as code, not an opening tag, and is a
 * syntax error. This guards against authors accidentally double-opening. */
file_put_contents($file, "<?php\necho \"this-must-fail\";\n");
register_shutdown_function(fn() => @unlink($file));
try {
    require $file;
} catch (\ParseError $e) {
    echo "ParseError: ", $e->getMessage(), "\n";
}
?>
--EXPECTF--
ParseError: syntax error, %s
