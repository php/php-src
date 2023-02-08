--TEST--
Collections\Deque unserialize error handling
--FILE--
<?php

call_user_func(function () {
    $it = new Collections\Deque(['first' => 'second']);
    $ser = 'O:17:"Collections\Deque":2:{i:0;s:5:"first";s:5:"unexp";s:6:"second";}';
    try {
        unserialize($ser);
    } catch (Throwable $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
});
?>
--EXPECT--
Caught UnexpectedValueException: Collections\Deque::__unserialize saw unexpected string key, expected sequence of values
