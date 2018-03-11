--TEST--
Flexible heredoc and nowdoc testing with token_get_all
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php

function output(array $tokens)
{
    foreach ($tokens as $token) {
        if (is_array($token)) {
            echo "Line {$token[2]}: ", token_name($token[0]), " ('{$token[1]}')", PHP_EOL;
        }
    }
}

output(token_get_all(<<<'OUTER_END'
<?php
echo <<<INNER_END
INNER_END;
OUTER_END));

echo PHP_EOL;

output(token_get_all(<<<'OUTER_END'
<?php
echo <<<INNER_END
  INNER_END;
OUTER_END));

echo PHP_EOL;

output(token_get_all(<<<'OUTER_END'
<?php
echo <<<'INNER_END'
INNER_END;
OUTER_END));

echo PHP_EOL;

output(token_get_all(<<<'OUTER_END'
<?php
echo <<<'INNER_END'
  INNER_END;
OUTER_END));

echo PHP_EOL;

output(token_get_all(<<<'OUTER_END'
  <?php
  echo <<<INNER_END
  a
  OUTER_END
));

echo PHP_EOL;

output(token_get_all(<<<'OUTER_END'
<?php
echo <<<INNER_END
a
OUTER_END
));

echo PHP_EOL;

output(token_get_all(<<<'OUTER_END'
<?php
echo <<<INNER_END
a
INNER_END;
OUTER_END, TOKEN_PARSE));

--EXPECT--
Line 1: T_OPEN_TAG ('<?php
')
Line 2: T_ECHO ('echo')
Line 2: T_WHITESPACE (' ')
Line 2: T_START_HEREDOC ('<<<INNER_END
')
Line 3: T_END_HEREDOC ('INNER_END')

Line 1: T_OPEN_TAG ('<?php
')
Line 2: T_ECHO ('echo')
Line 2: T_WHITESPACE (' ')
Line 2: T_START_HEREDOC ('<<<INNER_END
')
Line 3: T_END_HEREDOC ('INNER_END')

Line 1: T_OPEN_TAG ('<?php
')
Line 2: T_ECHO ('echo')
Line 2: T_WHITESPACE (' ')
Line 2: T_START_HEREDOC ('<<<'INNER_END'
')
Line 3: T_END_HEREDOC ('INNER_END')

Line 1: T_OPEN_TAG ('<?php
')
Line 2: T_ECHO ('echo')
Line 2: T_WHITESPACE (' ')
Line 2: T_START_HEREDOC ('<<<'INNER_END'
')
Line 3: T_END_HEREDOC ('INNER_END')

Line 1: T_OPEN_TAG ('<?php
')
Line 2: T_ECHO ('echo')
Line 2: T_WHITESPACE (' ')
Line 2: T_START_HEREDOC ('<<<INNER_END
')
Line 3: T_ENCAPSED_AND_WHITESPACE ('a')

Line 1: T_OPEN_TAG ('<?php
')
Line 2: T_ECHO ('echo')
Line 2: T_WHITESPACE (' ')
Line 2: T_START_HEREDOC ('<<<INNER_END
')
Line 3: T_ENCAPSED_AND_WHITESPACE ('a')

Line 1: T_OPEN_TAG ('<?php
')
Line 2: T_ECHO ('echo')
Line 2: T_WHITESPACE (' ')
Line 2: T_START_HEREDOC ('<<<INNER_END
')
Line 3: T_ENCAPSED_AND_WHITESPACE ('a
')
Line 4: T_END_HEREDOC ('INNER_END')