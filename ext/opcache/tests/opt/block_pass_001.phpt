--TEST--
Block Pass 001: QM_ASSIGN and DECLARE_LAMBDA_FUNCTION
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
abstract class Broadcaster {
    protected function normalizeChannelHandlerToCallable($callback)
        {
            return is_callable($callback) ? $callback : function (...$args) use ($callback) {
                return Container::getInstance()
                    ->make($callback)
                ->join(...$args);
        };
    }
}
?>
OK
--EXPECT--
OK
