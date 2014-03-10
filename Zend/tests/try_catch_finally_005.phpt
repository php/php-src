--TEST--
Try catch finally (break / cont in try block)
--CREDITS--
adoy
--FILE--
<?php
for ($i = 0;  $i < 100 ; $i ++) {
    try {
        break;
    } finally {
        var_dump("break");
    }
}


for ($i = 0;  $i < 2; $i ++) {
    try {
        continue;
    } finally {
        var_dump("continue1");
    }
}

for ($i = 0;  $i < 3; $i ++) {
    try {
        try {
            continue;
        } finally {
            var_dump("continue2");
            if ($i == 1) {
                throw new Exception("continue exception");
            }
        }
    } catch (Exception $e) {
       var_dump("cactched");
    }  finally {
       var_dump("finally");
    }
}

?>
--EXPECTF--
string(5) "break"
string(9) "continue1"
string(9) "continue1"
string(9) "continue2"
string(7) "finally"
string(9) "continue2"
string(8) "cactched"
string(7) "finally"
string(9) "continue2"
string(7) "finally"
