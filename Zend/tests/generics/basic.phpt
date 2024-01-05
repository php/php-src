--TEST--
Basic generic class declaration
--FILE--
<?php

interface I<T> {
}

class C<T> {
}

final class F<T> {
}

trait T<P> {
}

?>
--EXPECT--

