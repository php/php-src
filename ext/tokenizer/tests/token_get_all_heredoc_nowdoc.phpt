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

echo 'Test case 1', PHP_EOL, PHP_EOL;

output(token_get_all(<<<'OUTER_END'
<?php <<<INNER_END
INNER_END;
OUTER_END));

echo PHP_EOL, 'Test case 2', PHP_EOL, PHP_EOL;

output(token_get_all(<<<'OUTER_END'
<?php <<<INNER_END
  INNER_END;
OUTER_END));

echo PHP_EOL, 'Test case 3', PHP_EOL, PHP_EOL;

output(token_get_all(<<<'OUTER_END'
<?php <<<'INNER_END'
INNER_END;
OUTER_END));

echo PHP_EOL, 'Test case 4', PHP_EOL, PHP_EOL;

output(token_get_all(<<<'OUTER_END'
<?php <<<'INNER_END'
  INNER_END;
OUTER_END));

echo PHP_EOL, 'Test case 5', PHP_EOL, PHP_EOL;

output(token_get_all(<<<'OUTER_END'
  <?php <<<INNER_END
  a
  OUTER_END));

echo PHP_EOL, 'Test case 6', PHP_EOL, PHP_EOL;

output(token_get_all(<<<'OUTER_END'
<?php <<<INNER_END
ab
OUTER_END));

echo PHP_EOL, 'Test case 7', PHP_EOL, PHP_EOL;

output(token_get_all(<<<'OUTER_END'
<?php <<<INNER_END
a
INNER_END;
OUTER_END, TOKEN_PARSE));

echo PHP_EOL, 'Test case 8', PHP_EOL, PHP_EOL;

$code = <<<CODE
<?php
 \t<<<'DOC'
 \tXXX
 \tDOC;
CODE;
output(token_get_all($code));

echo PHP_EOL, 'Test case 9', PHP_EOL, PHP_EOL;

output(token_get_all(<<<'OUTER_END'
<?php <<<INNER_END
ab
INNER_END;
OUTER_END));

echo PHP_EOL, 'Test case 10', PHP_EOL, PHP_EOL;

output(token_get_all(<<<'OUTER_END'
<?php <<<INNER_END
  ab
  INNER_END;
OUTER_END));

echo PHP_EOL, 'Test case 11', PHP_EOL, PHP_EOL;

output(token_get_all(<<<'OUTER_END'
<?php <<<INNER_END
abc
   INNER_END;
OUTER_END));

echo PHP_EOL, 'Test case 12', PHP_EOL, PHP_EOL;

output(token_get_all(<<<'OUTER_END'
<?php <<<INNER_END

   INNER_END;
OUTER_END));

echo PHP_EOL, 'Test case 13', PHP_EOL, PHP_EOL;

output(token_get_all(<<<'OUTER_END'
<?php <<<'INNER_END'
ab
INNER_END;
OUTER_END));

echo PHP_EOL, 'Test case 14', PHP_EOL, PHP_EOL;

output(token_get_all(<<<'OUTER_END'
<?php <<<'INNER_END'
  ab
  INNER_END;
OUTER_END));

echo PHP_EOL, 'Test case 15', PHP_EOL, PHP_EOL;

output(token_get_all(<<<'OUTER_END'
<?php <<<'INNER_END'
abc
   INNER_END;
OUTER_END));

echo PHP_EOL, 'Test case 16', PHP_EOL, PHP_EOL;

output(token_get_all(<<<'OUTER_END'
<?php <<<'INNER_END'

   INNER_END;
OUTER_END));

echo PHP_EOL, 'Test case 17', PHP_EOL, PHP_EOL;

$code = <<<CODE
<?php
 \t<<<'DOC'
 \tXXX
 \tDOC;
CODE;
output(token_get_all($code, TOKEN_PARSE));

--EXPECT--
Test case 1

Line 1: T_OPEN_TAG ('<?php ')
Line 1: T_START_HEREDOC ('<<<INNER_END
')
Line 2: T_END_HEREDOC ('INNER_END')

Test case 2

Line 1: T_OPEN_TAG ('<?php ')
Line 1: T_START_HEREDOC ('<<<INNER_END
')
Line 2: T_END_HEREDOC ('  INNER_END')

Test case 3

Line 1: T_OPEN_TAG ('<?php ')
Line 1: T_START_HEREDOC ('<<<'INNER_END'
')
Line 2: T_END_HEREDOC ('INNER_END')

Test case 4

Line 1: T_OPEN_TAG ('<?php ')
Line 1: T_START_HEREDOC ('<<<'INNER_END'
')
Line 2: T_END_HEREDOC ('  INNER_END')

Test case 5

Line 1: T_OPEN_TAG ('<?php ')
Line 1: T_SL ('<<')
Line 1: T_STRING ('INNER_END')

Test case 6

Line 1: T_OPEN_TAG ('<?php ')
Line 1: T_START_HEREDOC ('<<<INNER_END
')
Line 2: T_ENCAPSED_AND_WHITESPACE ('ab')

Test case 7

Line 1: T_OPEN_TAG ('<?php ')
Line 1: T_START_HEREDOC ('<<<INNER_END
')
Line 2: T_ENCAPSED_AND_WHITESPACE ('a
')
Line 3: T_END_HEREDOC ('INNER_END')

Test case 8

Line 1: T_OPEN_TAG ('<?php
')
Line 2: T_WHITESPACE (' 	')
Line 2: T_START_HEREDOC ('<<<'DOC'
')
Line 3: T_ENCAPSED_AND_WHITESPACE (' 	XXX
')
Line 4: T_END_HEREDOC (' 	DOC')

Test case 9

Line 1: T_OPEN_TAG ('<?php ')
Line 1: T_START_HEREDOC ('<<<INNER_END
')
Line 2: T_ENCAPSED_AND_WHITESPACE ('ab
')
Line 3: T_END_HEREDOC ('INNER_END')

Test case 10

Line 1: T_OPEN_TAG ('<?php ')
Line 1: T_START_HEREDOC ('<<<INNER_END
')
Line 2: T_ENCAPSED_AND_WHITESPACE ('  ab
')
Line 3: T_END_HEREDOC ('  INNER_END')

Test case 11

Line 1: T_OPEN_TAG ('<?php ')
Line 1: T_START_HEREDOC ('<<<INNER_END
')
Line 2: T_ENCAPSED_AND_WHITESPACE ('abc
')
Line 3: T_END_HEREDOC ('   INNER_END')

Test case 12

Line 1: T_OPEN_TAG ('<?php ')
Line 1: T_START_HEREDOC ('<<<INNER_END
')
Line 2: T_ENCAPSED_AND_WHITESPACE ('
')
Line 3: T_END_HEREDOC ('   INNER_END')

Test case 13

Line 1: T_OPEN_TAG ('<?php ')
Line 1: T_START_HEREDOC ('<<<'INNER_END'
')
Line 2: T_ENCAPSED_AND_WHITESPACE ('ab
')
Line 3: T_END_HEREDOC ('INNER_END')

Test case 14

Line 1: T_OPEN_TAG ('<?php ')
Line 1: T_START_HEREDOC ('<<<'INNER_END'
')
Line 2: T_ENCAPSED_AND_WHITESPACE ('  ab
')
Line 3: T_END_HEREDOC ('  INNER_END')

Test case 15

Line 1: T_OPEN_TAG ('<?php ')
Line 1: T_START_HEREDOC ('<<<'INNER_END'
')
Line 2: T_ENCAPSED_AND_WHITESPACE ('abc
')
Line 3: T_END_HEREDOC ('   INNER_END')

Test case 16

Line 1: T_OPEN_TAG ('<?php ')
Line 1: T_START_HEREDOC ('<<<'INNER_END'
')
Line 2: T_ENCAPSED_AND_WHITESPACE ('
')
Line 3: T_END_HEREDOC ('   INNER_END')

Test case 17


Parse error: Invalid indentation - tabs and spaces cannot be mixed in  on line 3