--TEST--
Flexible heredoc/nowdoc syntax
--FILE--
<?php

$text = <<<END
    a
   b
  c
 d
e
END;

echo $text, PHP_EOL;

$text = <<<'END'
    a
   b
  c
 d
e
 END;

echo $text, PHP_EOL;

$text = <<<END
    a
   b
  c
 d
e
  END;

echo $text, PHP_EOL;

$text = <<<'END'
    a
   b
  c
 d
e
   END;

echo $text, PHP_EOL;

$text = <<<END
    a
   b
  c
 d
e
    END;

echo $text, PHP_EOL;

$text = <<<'END'
    a
   b
  c
 d
e
     END;

echo $text, PHP_EOL;

$text = <<<END

    a

   b

  c

 d

e

END;

echo $text, PHP_EOL;

$text = <<<'END'

    a

   b

  c

 d

e

     END;

echo $text, PHP_EOL;

$text = <<<END
	a\r\n
\ta\n
   b\r\n
  c\n
 d\r\n
e\n
 END;

echo $text, PHP_EOL;

echo <<<END
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