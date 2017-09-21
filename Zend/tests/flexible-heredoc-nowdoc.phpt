--TEST--
Flexible heredoc/nowdoc syntax
--FILE--
<?php

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
	  c
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
  c\n
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