--TEST--
Bug #76991: Incorrect tokenization of multiple invalid flexible heredoc strings
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip tokenizer extension not enabled"; ?>
--FILE--
<?php

$code = <<<CODE
<?php
<<<TEST
\$a
 TEST;

<<<END
\$a
 END;
CODE;
foreach (token_get_all($code) as $token) {
    if (is_array($token)) {
        echo token_name($token[0]) . ": " .str_replace("\n", '\n', $token[1]);
    } else {
        echo $token;
    }
    echo "\n";
}

?>
--EXPECT--
T_OPEN_TAG: <?php\n
T_START_HEREDOC: <<<TEST\n
T_VARIABLE: $a
T_ENCAPSED_AND_WHITESPACE: \n
T_END_HEREDOC:  TEST
;
T_WHITESPACE: \n\n
T_START_HEREDOC: <<<END\n
T_VARIABLE: $a
T_ENCAPSED_AND_WHITESPACE: \n
T_END_HEREDOC:  END
;
