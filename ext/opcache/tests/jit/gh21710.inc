<?php
if (getenv('call_user_func')) {
    eval('class P {}');
}

class C extends P {
    static function f($v) {
        return $v[0];
        if ($a) {
            return 1;
        } else {
            return 2;
        }
    }
}
