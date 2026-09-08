--TEST--
GH-23552 (keyword class constant name not retagged to T_STRING)
--EXTENSIONS--
tokenizer
--FILE--
<?php
$cases = [
    '<?php class F { const NEW = Bar::NEW; }',
    '<?php class F { const NEW = [self::NEW]; }',
    '<?php class F { const NEW = NEW Bar(); }',
    '<?php class F { const PRINT = X::PRINT; }',
    "<?php class F { const NEW = <<<EOT\n  a\n  EOT; const PRINT = Bar::PRINT; }",
];

foreach ($cases as $code) {
    echo $code, "\n";
    foreach (token_get_all($code, TOKEN_PARSE) as $token) {
        if (is_array($token) && in_array(strtolower($token[1]), ['new', 'print'], true)) {
            echo '  token_get_all: ', token_name($token[0]), ' [', $token[1], "]\n";
        }
    }
    foreach (PhpToken::tokenize($code, TOKEN_PARSE) as $token) {
        if (in_array(strtolower($token->text), ['new', 'print'], true)) {
            echo '  PhpToken: ', token_name($token->id), ' [', $token->text, '] at ', $token->pos, "\n";
        }
    }
}

try {
    token_get_all('<?php class F { const NEW = 1 /* unterminated', TOKEN_PARSE);
} catch (Throwable $e) {
    echo 'error path: ', $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
<?php class F { const NEW = Bar::NEW; }
  token_get_all: T_STRING [NEW]
  token_get_all: T_STRING [NEW]
  PhpToken: T_STRING [NEW] at 22
  PhpToken: T_STRING [NEW] at 33
<?php class F { const NEW = [self::NEW]; }
  token_get_all: T_STRING [NEW]
  token_get_all: T_STRING [NEW]
  PhpToken: T_STRING [NEW] at 22
  PhpToken: T_STRING [NEW] at 35
<?php class F { const NEW = NEW Bar(); }
  token_get_all: T_STRING [NEW]
  token_get_all: T_NEW [NEW]
  PhpToken: T_STRING [NEW] at 22
  PhpToken: T_NEW [NEW] at 28
<?php class F { const PRINT = X::PRINT; }
  token_get_all: T_STRING [PRINT]
  token_get_all: T_STRING [PRINT]
  PhpToken: T_STRING [PRINT] at 22
  PhpToken: T_STRING [PRINT] at 33
<?php class F { const NEW = <<<EOT
  a
  EOT; const PRINT = Bar::PRINT; }
  token_get_all: T_STRING [NEW]
  token_get_all: T_STRING [PRINT]
  token_get_all: T_STRING [PRINT]
  PhpToken: T_STRING [NEW] at 22
  PhpToken: T_STRING [PRINT] at 52
  PhpToken: T_STRING [PRINT] at 65
error path: ParseError: Unterminated comment starting line 1
