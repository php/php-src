--TEST--
Flexible heredoc/nowdoc syntax
--FILE--
<?php

$test = 'c';

var_dump(<<<'END'
 END);

var_dump(<<<END

  END);

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
?>
--EXPECT--
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