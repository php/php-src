--TEST--
Flexible heredoc and nowdoc testing with token_get_all
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php

function test(string $code, int $flags)
{
    try {
        $tokens = token_get_all($code, $flags);
        foreach ($tokens as $token) {
            if (is_array($token)) {
                echo "Line {$token[2]}: ", token_name($token[0]), " ('{$token[1]}')\n";
            }
        }
    } catch (ParseError $e) {
        echo "Parse error: {$e->getMessage()} on line {$e->getLine()}\n";
    }
}

$tests = [];

$tests[1] = <<<'OUTER_END'
<?php <<<INNER_END
INNER_END;
OUTER_END;

$tests[2] = <<<'OUTER_END'
<?php <<<INNER_END
  INNER_END;
OUTER_END;

$tests[3] = <<<'OUTER_END'
<?php <<<'INNER_END'
INNER_END;
OUTER_END;

$tests[4] = <<<'OUTER_END'
<?php <<<'INNER_END'
  INNER_END;
OUTER_END;

$tests[5] = <<<'OUTER_END'
  <?php <<<INNER_END
  a
  OUTER_END;

$tests[6] = <<<'OUTER_END'
<?php <<<INNER_END
ab
OUTER_END;

$tests[7] = <<<'OUTER_END'
<?php <<<INNER_END
a
INNER_END;
OUTER_END;

$tests[8] = <<<CODE
<?php
 \t<<<'DOC'
 \tXXX
 \tDOC;
CODE;

$tests[9] = <<<'OUTER_END'
<?php <<<INNER_END
ab
INNER_END;
OUTER_END;

$tests[10] = <<<'OUTER_END'
<?php <<<INNER_END
  ab
  INNER_END;
OUTER_END;

$tests[11] = <<<'OUTER_END'
<?php <<<INNER_END
abc
   INNER_END;
OUTER_END;

$tests[12] = <<<'OUTER_END'
<?php <<<INNER_END

   INNER_END;
OUTER_END;

$tests[13] = <<<'OUTER_END'
<?php <<<'INNER_END'
ab
INNER_END;
OUTER_END;

$tests[14] = <<<'OUTER_END'
<?php <<<'INNER_END'
  ab
  INNER_END;
OUTER_END;

$tests[15] = <<<'OUTER_END'
<?php <<<'INNER_END'
abc
   INNER_END;
OUTER_END;

$tests[16] = <<<'OUTER_END'
<?php <<<'INNER_END'

   INNER_END;
OUTER_END;

$tests[17] = <<<OUTER_END
<?php <<<INNER_END
ab
ab
\t
OUTER_END;

$tests[18] = <<<OUTER_END
<?php <<<'INNER_END'
ab
ab
\t
OUTER_END;

echo "Without TOKEN_PARSE:\n";
foreach ($tests as $i => $test) {
    echo "\nTest case $i\n\n";
    test($test, 0);
}

echo "\nWith TOKEN_PARSE:\n";
foreach ($tests as $i => $test) {
    echo "\nTest case $i\n\n";
    test($test, TOKEN_PARSE);
}

?>
--EXPECT--
Without TOKEN_PARSE:

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
Line 1: T_START_HEREDOC ('<<<INNER_END
')
Line 2: T_ENCAPSED_AND_WHITESPACE ('a')

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

Line 1: T_OPEN_TAG ('<?php ')
Line 1: T_START_HEREDOC ('<<<INNER_END
')
Line 2: T_ENCAPSED_AND_WHITESPACE ('ab
ab
	')

Test case 18

Line 1: T_OPEN_TAG ('<?php ')
Line 1: T_START_HEREDOC ('<<<'INNER_END'
')
Line 2: T_ENCAPSED_AND_WHITESPACE ('ab
ab
	')

With TOKEN_PARSE:

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

Parse error: syntax error, unexpected end of file, expecting variable (T_VARIABLE) or heredoc end (T_END_HEREDOC) or ${ (T_DOLLAR_OPEN_CURLY_BRACES) or {$ (T_CURLY_OPEN) on line 2

Test case 6

Parse error: syntax error, unexpected end of file, expecting variable (T_VARIABLE) or heredoc end (T_END_HEREDOC) or ${ (T_DOLLAR_OPEN_CURLY_BRACES) or {$ (T_CURLY_OPEN) on line 2

Test case 7

Line 1: T_OPEN_TAG ('<?php ')
Line 1: T_START_HEREDOC ('<<<INNER_END
')
Line 2: T_ENCAPSED_AND_WHITESPACE ('a
')
Line 3: T_END_HEREDOC ('INNER_END')

Test case 8

Parse error: Invalid indentation - tabs and spaces cannot be mixed on line 3

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

Parse error: Invalid body indentation level (expecting an indentation level of at least 3) on line 2

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

Parse error: Invalid body indentation level (expecting an indentation level of at least 3) on line 2

Test case 16

Line 1: T_OPEN_TAG ('<?php ')
Line 1: T_START_HEREDOC ('<<<'INNER_END'
')
Line 2: T_ENCAPSED_AND_WHITESPACE ('
')
Line 3: T_END_HEREDOC ('   INNER_END')

Test case 17

Parse error: syntax error, unexpected end of file, expecting variable (T_VARIABLE) or heredoc end (T_END_HEREDOC) or ${ (T_DOLLAR_OPEN_CURLY_BRACES) or {$ (T_CURLY_OPEN) on line 4

Test case 18

Parse error: syntax error, unexpected end of file, expecting variable (T_VARIABLE) or heredoc end (T_END_HEREDOC) or ${ (T_DOLLAR_OPEN_CURLY_BRACES) or {$ (T_CURLY_OPEN) on line 4
