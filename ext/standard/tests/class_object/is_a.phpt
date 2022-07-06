--TEST--
is_a and is_subclass_of behaviour (with and without autoload)
--FILE--
<?php

interface if_a {
    function f_a();
}

interface if_b extends if_a {
    function f_b();
}

class base {
    function _is_a($sub) {

        echo "\n>>> With Defined class\n";
        echo str_pad('is_a( OBJECT:'.get_class($this).', '.$sub.') = ', 60) . (is_a($this, $sub) ? 'yes' : 'no')."\n";
        echo str_pad('is_a( STRING:'.get_class($this).', '.$sub.') = ', 60). (is_a(get_class($this), $sub) ? 'yes' : 'no')."\n";
        echo str_pad('is_a( STRING:'.get_class($this).', '.$sub.', true) = ', 60). (is_a(get_class($this), $sub, true) ? 'yes' : 'no')."\n";
        echo str_pad('is_subclass_of( OBJECT:'.get_class($this).', '.$sub.') = ', 60).  (is_subclass_of($this, $sub) ? 'yes' : 'no')."\n";
        echo str_pad('is_subclass_of( STRING:'.get_class($this).', '.$sub.') = ', 60). (is_subclass_of(get_class($this), $sub) ? 'yes' : 'no')."\n";
        echo str_pad('is_subclass_of( STRING:'.get_class($this).', '.$sub.',false) = ', 60). (is_subclass_of(get_class($this), $sub , false) ? 'yes' : 'no')."\n";

        // with autoload options..
        echo ">>> With Undefined\n";
        echo  str_pad('is_a( STRING:undefB, '.$sub.',true) = ', 60). (is_a('undefB', $sub, true) ? 'yes' : 'no')."\n";
        echo  str_pad('is_a( STRING:undefB, '.$sub.') = ', 60). (is_a('undefB', $sub) ? 'yes' : 'no')."\n";
        echo  str_pad('is_subclass_of( STRING:undefB, '.$sub.',false) = ', 60). (is_subclass_of('undefB', $sub, false) ? 'yes' : 'no')."\n";
        echo  str_pad('is_subclass_of( STRING:undefB, '.$sub.') = ', 60). (is_subclass_of('undefB', $sub) ? 'yes' : 'no')."\n";
    }
    function test() {
        echo $this->_is_a('base');
        echo $this->_is_a('derived_a');
        echo $this->_is_a('if_a');
        echo $this->_is_a('undefA');
        echo "\n";
    }
}

class derived_a extends base implements if_a {
    function f_a() {}
}

class derived_b extends base implements if_a, if_b {
    function f_a() {}
    function f_b() {}
}

class derived_c extends derived_a implements if_b {
    function f_b() {}
}

class derived_d extends derived_c {
}

$t = new base();
$t->test();

$t = new derived_a();
$t->test();

spl_autoload_register(function ($name) {
    echo ">>>> In autoload: ";
    var_dump($name);
});

echo "NOW WITH AUTOLOAD\n\n";

$t = new base();
$t->test();

$t = new derived_a();
$t->test();

$t = new derived_b();
$t->test();





?>
--EXPECT--
>>> With Defined class
is_a( OBJECT:base, base) =                                  yes
is_a( STRING:base, base) =                                  no
is_a( STRING:base, base, true) =                            yes
is_subclass_of( OBJECT:base, base) =                        no
is_subclass_of( STRING:base, base) =                        no
is_subclass_of( STRING:base, base,false) =                  no
>>> With Undefined
is_a( STRING:undefB, base,true) =                           no
is_a( STRING:undefB, base) =                                no
is_subclass_of( STRING:undefB, base,false) =                no
is_subclass_of( STRING:undefB, base) =                      no

>>> With Defined class
is_a( OBJECT:base, derived_a) =                             no
is_a( STRING:base, derived_a) =                             no
is_a( STRING:base, derived_a, true) =                       no
is_subclass_of( OBJECT:base, derived_a) =                   no
is_subclass_of( STRING:base, derived_a) =                   no
is_subclass_of( STRING:base, derived_a,false) =             no
>>> With Undefined
is_a( STRING:undefB, derived_a,true) =                      no
is_a( STRING:undefB, derived_a) =                           no
is_subclass_of( STRING:undefB, derived_a,false) =           no
is_subclass_of( STRING:undefB, derived_a) =                 no

>>> With Defined class
is_a( OBJECT:base, if_a) =                                  no
is_a( STRING:base, if_a) =                                  no
is_a( STRING:base, if_a, true) =                            no
is_subclass_of( OBJECT:base, if_a) =                        no
is_subclass_of( STRING:base, if_a) =                        no
is_subclass_of( STRING:base, if_a,false) =                  no
>>> With Undefined
is_a( STRING:undefB, if_a,true) =                           no
is_a( STRING:undefB, if_a) =                                no
is_subclass_of( STRING:undefB, if_a,false) =                no
is_subclass_of( STRING:undefB, if_a) =                      no

>>> With Defined class
is_a( OBJECT:base, undefA) =                                no
is_a( STRING:base, undefA) =                                no
is_a( STRING:base, undefA, true) =                          no
is_subclass_of( OBJECT:base, undefA) =                      no
is_subclass_of( STRING:base, undefA) =                      no
is_subclass_of( STRING:base, undefA,false) =                no
>>> With Undefined
is_a( STRING:undefB, undefA,true) =                         no
is_a( STRING:undefB, undefA) =                              no
is_subclass_of( STRING:undefB, undefA,false) =              no
is_subclass_of( STRING:undefB, undefA) =                    no


>>> With Defined class
is_a( OBJECT:derived_a, base) =                             yes
is_a( STRING:derived_a, base) =                             no
is_a( STRING:derived_a, base, true) =                       yes
is_subclass_of( OBJECT:derived_a, base) =                   yes
is_subclass_of( STRING:derived_a, base) =                   yes
is_subclass_of( STRING:derived_a, base,false) =             no
>>> With Undefined
is_a( STRING:undefB, base,true) =                           no
is_a( STRING:undefB, base) =                                no
is_subclass_of( STRING:undefB, base,false) =                no
is_subclass_of( STRING:undefB, base) =                      no

>>> With Defined class
is_a( OBJECT:derived_a, derived_a) =                        yes
is_a( STRING:derived_a, derived_a) =                        no
is_a( STRING:derived_a, derived_a, true) =                  yes
is_subclass_of( OBJECT:derived_a, derived_a) =              no
is_subclass_of( STRING:derived_a, derived_a) =              no
is_subclass_of( STRING:derived_a, derived_a,false) =        no
>>> With Undefined
is_a( STRING:undefB, derived_a,true) =                      no
is_a( STRING:undefB, derived_a) =                           no
is_subclass_of( STRING:undefB, derived_a,false) =           no
is_subclass_of( STRING:undefB, derived_a) =                 no

>>> With Defined class
is_a( OBJECT:derived_a, if_a) =                             yes
is_a( STRING:derived_a, if_a) =                             no
is_a( STRING:derived_a, if_a, true) =                       yes
is_subclass_of( OBJECT:derived_a, if_a) =                   yes
is_subclass_of( STRING:derived_a, if_a) =                   yes
is_subclass_of( STRING:derived_a, if_a,false) =             no
>>> With Undefined
is_a( STRING:undefB, if_a,true) =                           no
is_a( STRING:undefB, if_a) =                                no
is_subclass_of( STRING:undefB, if_a,false) =                no
is_subclass_of( STRING:undefB, if_a) =                      no

>>> With Defined class
is_a( OBJECT:derived_a, undefA) =                           no
is_a( STRING:derived_a, undefA) =                           no
is_a( STRING:derived_a, undefA, true) =                     no
is_subclass_of( OBJECT:derived_a, undefA) =                 no
is_subclass_of( STRING:derived_a, undefA) =                 no
is_subclass_of( STRING:derived_a, undefA,false) =           no
>>> With Undefined
is_a( STRING:undefB, undefA,true) =                         no
is_a( STRING:undefB, undefA) =                              no
is_subclass_of( STRING:undefB, undefA,false) =              no
is_subclass_of( STRING:undefB, undefA) =                    no

NOW WITH AUTOLOAD


>>> With Defined class
is_a( OBJECT:base, base) =                                  yes
is_a( STRING:base, base) =                                  no
is_a( STRING:base, base, true) =                            yes
is_subclass_of( OBJECT:base, base) =                        no
is_subclass_of( STRING:base, base) =                        no
is_subclass_of( STRING:base, base,false) =                  no
>>> With Undefined
>>>> In autoload: string(6) "undefB"
is_a( STRING:undefB, base,true) =                           no
is_a( STRING:undefB, base) =                                no
is_subclass_of( STRING:undefB, base,false) =                no
>>>> In autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, base) =                      no

>>> With Defined class
is_a( OBJECT:base, derived_a) =                             no
is_a( STRING:base, derived_a) =                             no
is_a( STRING:base, derived_a, true) =                       no
is_subclass_of( OBJECT:base, derived_a) =                   no
is_subclass_of( STRING:base, derived_a) =                   no
is_subclass_of( STRING:base, derived_a,false) =             no
>>> With Undefined
>>>> In autoload: string(6) "undefB"
is_a( STRING:undefB, derived_a,true) =                      no
is_a( STRING:undefB, derived_a) =                           no
is_subclass_of( STRING:undefB, derived_a,false) =           no
>>>> In autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, derived_a) =                 no

>>> With Defined class
is_a( OBJECT:base, if_a) =                                  no
is_a( STRING:base, if_a) =                                  no
is_a( STRING:base, if_a, true) =                            no
is_subclass_of( OBJECT:base, if_a) =                        no
is_subclass_of( STRING:base, if_a) =                        no
is_subclass_of( STRING:base, if_a,false) =                  no
>>> With Undefined
>>>> In autoload: string(6) "undefB"
is_a( STRING:undefB, if_a,true) =                           no
is_a( STRING:undefB, if_a) =                                no
is_subclass_of( STRING:undefB, if_a,false) =                no
>>>> In autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, if_a) =                      no

>>> With Defined class
is_a( OBJECT:base, undefA) =                                no
is_a( STRING:base, undefA) =                                no
is_a( STRING:base, undefA, true) =                          no
is_subclass_of( OBJECT:base, undefA) =                      no
is_subclass_of( STRING:base, undefA) =                      no
is_subclass_of( STRING:base, undefA,false) =                no
>>> With Undefined
>>>> In autoload: string(6) "undefB"
is_a( STRING:undefB, undefA,true) =                         no
is_a( STRING:undefB, undefA) =                              no
is_subclass_of( STRING:undefB, undefA,false) =              no
>>>> In autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, undefA) =                    no


>>> With Defined class
is_a( OBJECT:derived_a, base) =                             yes
is_a( STRING:derived_a, base) =                             no
is_a( STRING:derived_a, base, true) =                       yes
is_subclass_of( OBJECT:derived_a, base) =                   yes
is_subclass_of( STRING:derived_a, base) =                   yes
is_subclass_of( STRING:derived_a, base,false) =             no
>>> With Undefined
>>>> In autoload: string(6) "undefB"
is_a( STRING:undefB, base,true) =                           no
is_a( STRING:undefB, base) =                                no
is_subclass_of( STRING:undefB, base,false) =                no
>>>> In autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, base) =                      no

>>> With Defined class
is_a( OBJECT:derived_a, derived_a) =                        yes
is_a( STRING:derived_a, derived_a) =                        no
is_a( STRING:derived_a, derived_a, true) =                  yes
is_subclass_of( OBJECT:derived_a, derived_a) =              no
is_subclass_of( STRING:derived_a, derived_a) =              no
is_subclass_of( STRING:derived_a, derived_a,false) =        no
>>> With Undefined
>>>> In autoload: string(6) "undefB"
is_a( STRING:undefB, derived_a,true) =                      no
is_a( STRING:undefB, derived_a) =                           no
is_subclass_of( STRING:undefB, derived_a,false) =           no
>>>> In autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, derived_a) =                 no

>>> With Defined class
is_a( OBJECT:derived_a, if_a) =                             yes
is_a( STRING:derived_a, if_a) =                             no
is_a( STRING:derived_a, if_a, true) =                       yes
is_subclass_of( OBJECT:derived_a, if_a) =                   yes
is_subclass_of( STRING:derived_a, if_a) =                   yes
is_subclass_of( STRING:derived_a, if_a,false) =             no
>>> With Undefined
>>>> In autoload: string(6) "undefB"
is_a( STRING:undefB, if_a,true) =                           no
is_a( STRING:undefB, if_a) =                                no
is_subclass_of( STRING:undefB, if_a,false) =                no
>>>> In autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, if_a) =                      no

>>> With Defined class
is_a( OBJECT:derived_a, undefA) =                           no
is_a( STRING:derived_a, undefA) =                           no
is_a( STRING:derived_a, undefA, true) =                     no
is_subclass_of( OBJECT:derived_a, undefA) =                 no
is_subclass_of( STRING:derived_a, undefA) =                 no
is_subclass_of( STRING:derived_a, undefA,false) =           no
>>> With Undefined
>>>> In autoload: string(6) "undefB"
is_a( STRING:undefB, undefA,true) =                         no
is_a( STRING:undefB, undefA) =                              no
is_subclass_of( STRING:undefB, undefA,false) =              no
>>>> In autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, undefA) =                    no


>>> With Defined class
is_a( OBJECT:derived_b, base) =                             yes
is_a( STRING:derived_b, base) =                             no
is_a( STRING:derived_b, base, true) =                       yes
is_subclass_of( OBJECT:derived_b, base) =                   yes
is_subclass_of( STRING:derived_b, base) =                   yes
is_subclass_of( STRING:derived_b, base,false) =             no
>>> With Undefined
>>>> In autoload: string(6) "undefB"
is_a( STRING:undefB, base,true) =                           no
is_a( STRING:undefB, base) =                                no
is_subclass_of( STRING:undefB, base,false) =                no
>>>> In autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, base) =                      no

>>> With Defined class
is_a( OBJECT:derived_b, derived_a) =                        no
is_a( STRING:derived_b, derived_a) =                        no
is_a( STRING:derived_b, derived_a, true) =                  no
is_subclass_of( OBJECT:derived_b, derived_a) =              no
is_subclass_of( STRING:derived_b, derived_a) =              no
is_subclass_of( STRING:derived_b, derived_a,false) =        no
>>> With Undefined
>>>> In autoload: string(6) "undefB"
is_a( STRING:undefB, derived_a,true) =                      no
is_a( STRING:undefB, derived_a) =                           no
is_subclass_of( STRING:undefB, derived_a,false) =           no
>>>> In autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, derived_a) =                 no

>>> With Defined class
is_a( OBJECT:derived_b, if_a) =                             yes
is_a( STRING:derived_b, if_a) =                             no
is_a( STRING:derived_b, if_a, true) =                       yes
is_subclass_of( OBJECT:derived_b, if_a) =                   yes
is_subclass_of( STRING:derived_b, if_a) =                   yes
is_subclass_of( STRING:derived_b, if_a,false) =             no
>>> With Undefined
>>>> In autoload: string(6) "undefB"
is_a( STRING:undefB, if_a,true) =                           no
is_a( STRING:undefB, if_a) =                                no
is_subclass_of( STRING:undefB, if_a,false) =                no
>>>> In autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, if_a) =                      no

>>> With Defined class
is_a( OBJECT:derived_b, undefA) =                           no
is_a( STRING:derived_b, undefA) =                           no
is_a( STRING:derived_b, undefA, true) =                     no
is_subclass_of( OBJECT:derived_b, undefA) =                 no
is_subclass_of( STRING:derived_b, undefA) =                 no
is_subclass_of( STRING:derived_b, undefA,false) =           no
>>> With Undefined
>>>> In autoload: string(6) "undefB"
is_a( STRING:undefB, undefA,true) =                         no
is_a( STRING:undefB, undefA) =                              no
is_subclass_of( STRING:undefB, undefA,false) =              no
>>>> In autoload: string(6) "undefB"
is_subclass_of( STRING:undefB, undefA) =                    no
