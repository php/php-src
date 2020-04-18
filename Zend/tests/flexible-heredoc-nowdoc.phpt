--TEST--
Flexible heredoc/nowdoc syntax
--FILE--
<?php

$test = 'c';

var_dump(<<<'END'
 END);

var_dump(<<<END

  END);

// Insufficient indentation is fine if the line is whitespace-only
// Using eval() here to avoid issue with trailing whitespace trimming
var_dump(eval("return <<<END
\x20
\x20\x20END;"));

echo <<<'END'
     a
    b

   c

  d
 e
 END, PHP_EOL;

echo <<<END
	    a
	   b
	  $test
	 d
	e
	END, PHP_EOL;

echo <<<'END'

    a

   b

  c

 d

e

END, PHP_EOL;

echo <<<END
	a\r\n
\ta\n
   b\r\n
  $test\n
 d\r\n
e\n
END, PHP_EOL;

echo <<<'END'
    a
   b
  c
 d
e
END, PHP_EOL;

$var = 'Bar';
var_dump(<<<TEST
$var
TEST);

$var = 'Bar';
var_dump(<<<TEST

$var
TEST);

?>
--EXPECT--
string(0) ""
string(0) ""
string(0) ""
    a
   b

  c

 d
e
    a
   b
  c
 d
e

    a

   b

  c

 d

e

	a

	a

   b

  c

 d

e

    a
   b
  c
 d
e
string(3) "Bar"
string(4) "
Bar"
