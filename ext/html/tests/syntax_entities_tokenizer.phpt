--TEST--
Markup syntax: token_get_all() keeps character references raw (source round-trips)
--EXTENSIONS--
html
tokenizer
--FILE--
<?php
// Entity decoding happens only for the compiler; the tokenizer must see the
// raw source text so token streams concatenate back to the original file.
$code = '<?php echo <p title="Caf&eacute;">Fish &amp; chips &#x2764;</p>; ?>';

$joined = '';
foreach (token_get_all($code) as $token) {
    $joined .= is_array($token) ? $token[1] : $token;
}
var_dump($joined === $code);
?>
--EXPECT--
bool(true)
