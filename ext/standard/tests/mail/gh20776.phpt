--TEST--
GH-20776: mail() memory leak when header array contains numeric keys
--FILE--
<?php
$to = "user@example.com";
$subject = $message = "";

try {
    var_dump(mail($to, $subject, $message, ['RandomHeader' => 'Value', 5 => 'invalid key']));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
TypeError: Header name cannot be numeric, 5 given
