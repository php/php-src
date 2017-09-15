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
\ta\r\n
   b\n
  c\r\n
 d\n
e\r\n
 END;

echo $text, PHP_EOL;
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

  b

 c

d

e
