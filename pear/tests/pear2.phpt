--TEST--
PEAR complete set/push/pop error-handling test (run from pear/tests dir)
--SKIPIF--
<?php
if (!getenv('PHP_PEAR_RUNTESTS')) {
    echo 'skip';
}
?>
--FILE--
<?php

require_once "PEAR.php";

class testErrorHandlingStatic {
    function doSetErrorHandlingFunction()
    {
        print "testing in class setErrorHandling\n";
        PEAR::setErrorHandling(PEAR_ERROR_CALLBACK, 'get_error_mode');
        echoPEARVars('_PEAR_default_error_mode', $GLOBALS['_PEAR_default_error_mode'],
                     '_PEAR_default_error_options', $GLOBALS['_PEAR_default_error_options']);
    }
    
    function doSetErrorHandlingStatic()
    {
        print "testing in class setErrorHandling array(obj, method)\n";
        $obj = new testErrorHandlingPEAR;
        PEAR::setErrorHandling(PEAR_ERROR_CALLBACK, array(&$obj, 'fakeHandleError'));
        echoPEARVars('_PEAR_default_error_mode', $GLOBALS['_PEAR_default_error_mode'],
                     '_PEAR_default_error_options', $GLOBALS['_PEAR_default_error_options']);
    }
    
    function doSetErrorHandlingObject()
    {
        print "testing in class setErrorHandling array(class, method)\n";
        PEAR::setErrorHandling(PEAR_ERROR_CALLBACK, array('testErrorHandlingStatic', 'fakeHandleError'));
        echoPEARVars('_PEAR_default_error_mode', $GLOBALS['_PEAR_default_error_mode'],
                     '_PEAR_default_error_options', $GLOBALS['_PEAR_default_error_options']);
    }

    function doPushErrorHandlingFunction()
    {
        print "testing in class pushErrorHandling\n";
        PEAR::pushErrorHandling(PEAR_ERROR_CALLBACK, 'get_error_mode');
        echoPEARStack('_PEAR_error_handler_stack', $GLOBALS['_PEAR_error_handler_stack']);
    }
    
    function doPushErrorHandlingObject()
    {
        print "testing in class pushErrorHandling array(obj, method)\n";
        $obj = new testErrorHandlingPEAR;
        PEAR::pushErrorHandling(PEAR_ERROR_CALLBACK, array(&$obj, 'fakeHandleError'));
        echoPEARStack('_PEAR_error_handler_stack', $GLOBALS['_PEAR_error_handler_stack']);
    }
    
    function doPushErrorHandlingStatic()
    {
        print "testing in class pushErrorHandling array(class, method)\n";
        PEAR::pushErrorHandling(PEAR_ERROR_CALLBACK, array('testErrorHandlingStatic', 'fakeHandleError'));
        echoPEARStack('_PEAR_error_handler_stack', $GLOBALS['_PEAR_error_handler_stack']);
    }
    
    function doPopErrorHandling()
    {
        print "testing in class popErrorHandling\n";
        PEAR::popErrorHandling();
        echoPEARStack('_PEAR_error_handler_stack', $GLOBALS['_PEAR_error_handler_stack']);
    }
    
    function fakeHandleError($err)
    {
    }
}

class testErrorHandlingPEAR extends PEAR {
    
    function fakeHandleError($err)
    {
    }

    function doSetErrorHandlingFunction()
    {
        print "testing in PEAR setErrorHandling\n";
        PEAR::setErrorHandling(PEAR_ERROR_CALLBACK, 'get_error_mode');
        echoPEARVars('_PEAR_default_error_mode', $GLOBALS['_PEAR_default_error_mode'],
                     '_PEAR_default_error_options', $GLOBALS['_PEAR_default_error_options']);
        echoPEARVars('$this->_default_error_mode', $this->_default_error_mode,
                     '$this->_default_error_options', $this->_default_error_options);
    }
    
    function doSetErrorHandlingStatic()
    {
        print "testing in PEAR setErrorHandling array(obj, method)\n";
        $obj = new testErrorHandlingPEAR;
        PEAR::setErrorHandling(PEAR_ERROR_CALLBACK, array(&$obj, 'fakeHandleError'));
        echoPEARVars('_PEAR_default_error_mode', $GLOBALS['_PEAR_default_error_mode'],
                     '_PEAR_default_error_options', $GLOBALS['_PEAR_default_error_options']);
        echoPEARVars('$this->_default_error_mode', $this->_default_error_mode,
                     '$this->_default_error_options', $this->_default_error_options);
    }
    
    function doSetErrorHandlingObject()
    {
        print "testing in PEAR setErrorHandling array(class, method)\n";
        PEAR::setErrorHandling(PEAR_ERROR_CALLBACK, array('testErrorHandlingStatic', 'fakeHandleError'));
        echoPEARVars('_PEAR_default_error_mode', $GLOBALS['_PEAR_default_error_mode'],
                     '_PEAR_default_error_options', $GLOBALS['_PEAR_default_error_options']);
        echoPEARVars('$this->_default_error_mode', $this->_default_error_mode,
                     '$this->_default_error_options', $this->_default_error_options);
    }

    function doPushErrorHandlingFunction()
    {
        print "testing in PEAR pushErrorHandling\n";
        PEAR::pushErrorHandling(PEAR_ERROR_CALLBACK, 'get_error_mode');
        echoPEARStack('_PEAR_error_handler_stack', $GLOBALS['_PEAR_error_handler_stack']);
    }
    
    function doPushErrorHandlingObject()
    {
        print "testing in PEAR pushErrorHandling array(obj, method)\n";
        $obj = new testErrorHandlingPEAR;
        PEAR::pushErrorHandling(PEAR_ERROR_CALLBACK, array(&$obj, 'fakeHandleError'));
        echoPEARStack('_PEAR_error_handler_stack', $GLOBALS['_PEAR_error_handler_stack']);
    }
    
    function doPushErrorHandlingStatic()
    {
        print "testing in PEAR pushErrorHandling array(class, method)\n";
        PEAR::pushErrorHandling(PEAR_ERROR_CALLBACK, array('testErrorHandlingStatic', 'fakeHandleError'));
        echoPEARStack('_PEAR_error_handler_stack', $GLOBALS['_PEAR_error_handler_stack']);
    }
    
    function doPopErrorHandling()
    {
        print "testing in PEAR popErrorHandling\n";
        PEAR::popErrorHandling();
        echoPEARStack('_PEAR_error_handler_stack', $GLOBALS['_PEAR_error_handler_stack']);
    }
}

function echoPEARVars($name1, $mode, $name2, $options, $indent = '')
{
    $levelMap = 
        array(
            E_USER_NOTICE => 'E_USER_NOTICE',
            E_USER_WARNING => 'E_USER_WARNING',
            E_USER_ERROR => 'E_USER_ERROR',
        );
    $pearLevelMap =
        array(
            PEAR_ERROR_RETURN =>'PEAR_ERROR_RETURN',
            PEAR_ERROR_PRINT =>'PEAR_ERROR_PRINT',
            PEAR_ERROR_TRIGGER =>'PEAR_ERROR_TRIGGER',
            PEAR_ERROR_DIE =>'PEAR_ERROR_DIE',
            PEAR_ERROR_CALLBACK =>'PEAR_ERROR_CALLBACK',
            PEAR_ERROR_EXCEPTION =>'PEAR_ERROR_EXCEPTION',
        );
    print $indent . "echoing PEAR error-handling Variables:\n";
    print "$indent--------------------------------------\n";
    print $indent . "$name1:\n";
    $levels = get_error_mode($mode);
    print $indent;
    foreach($levels as $level) {
        print $pearLevelMap[$level] . ',';
    }
    print "\n";
    print $indent . "$name2:\n";
    if (is_string($options)) {
        print $indent . 'Callback: ' . $options. "()\n";
    } elseif (is_array($options)) {
        print $indent . 'Callback: ';
        if (is_string($options[0])) {
            print '(static) ' . $options[0] . '::';
        } else {
            print get_class($options[0]) . '->';
        }
        print $options[1] . "()\n";
    } else {
        print $indent . $levelMap[$options] . "\n";
    }
    print "$indent--------------------------------------\n";
}

function echoPEARStack($name, $stack)
{
    print "stack $name:\n";
    foreach ($stack as $i =>  $item) {
        print "Index $i:\n";
        echoPEARVars('mode', $item[0], 'options', $item[1], '    ');
    }
}

function get_error_mode($err)
{
    $ret = array();
    $level = 0;
    while($level < 7) {
        $a = ($err >> $level++) & 1;
        if ($a) {
            $ret[] = 1 << ($level - 1);
        }
    }
    return $ret;
}
print "testing static error-handling global code\n";
echoPEARVars('_PEAR_default_error_mode', $GLOBALS['_PEAR_default_error_mode'],
             '_PEAR_default_error_options', $GLOBALS['_PEAR_default_error_options']);
print "testing setErrorHandling\n";
PEAR::setErrorHandling(PEAR_ERROR_CALLBACK, 'get_error_mode');
echoPEARVars('_PEAR_default_error_mode', $GLOBALS['_PEAR_default_error_mode'],
             '_PEAR_default_error_options', $GLOBALS['_PEAR_default_error_options']);
print "testing setErrorHandling array(obj, method)\n";
$obj = new testErrorHandlingPEAR;
PEAR::setErrorHandling(PEAR_ERROR_CALLBACK, array(&$obj, 'fakeHandleError'));
echoPEARVars('_PEAR_default_error_mode', $GLOBALS['_PEAR_default_error_mode'],
             '_PEAR_default_error_options', $GLOBALS['_PEAR_default_error_options']);
print "testing setErrorHandling array(class, method)\n";
PEAR::setErrorHandling(PEAR_ERROR_CALLBACK, array('testErrorHandlingStatic', 'fakeHandleError'));
echoPEARVars('_PEAR_default_error_mode', $GLOBALS['_PEAR_default_error_mode'],
             '_PEAR_default_error_options', $GLOBALS['_PEAR_default_error_options']);


print "testing pushErrorHandling\n";
PEAR::pushErrorHandling(PEAR_ERROR_CALLBACK, 'get_error_mode');
echoPEARStack('_PEAR_error_handler_stack', $GLOBALS['_PEAR_error_handler_stack']);
print "testing pushErrorHandling array(obj, method)\n";
$obj = new testErrorHandlingPEAR;
PEAR::pushErrorHandling(PEAR_ERROR_CALLBACK, array(&$obj, 'fakeHandleError'));
echoPEARStack('_PEAR_error_handler_stack', $GLOBALS['_PEAR_error_handler_stack']);
print "testing pushErrorHandling array(class, method)\n";
PEAR::pushErrorHandling(PEAR_ERROR_CALLBACK, array('testErrorHandlingStatic', 'fakeHandleError'));
echoPEARStack('_PEAR_error_handler_stack', $GLOBALS['_PEAR_error_handler_stack']);


print "testing popErrorHandling\n";
PEAR::popErrorHandling();
echoPEARStack('_PEAR_error_handler_stack', $GLOBALS['_PEAR_error_handler_stack']);
print "testing popErrorHandling\n";
$obj = new testErrorHandlingPEAR;
PEAR::popErrorHandling();
echoPEARStack('_PEAR_error_handler_stack', $GLOBALS['_PEAR_error_handler_stack']);
print "testing popErrorHandling\n";
PEAR::popErrorHandling();
echoPEARStack('_PEAR_error_handler_stack', $GLOBALS['_PEAR_error_handler_stack']);
print "*******************************************\n";
print "testing static error-handling in-class code\n";
print "*******************************************\n";
PEAR::setErrorHandling(PEAR_ERROR_RETURN, E_USER_NOTICE);
$obj = new testErrorHandlingStatic;
$obj->doSetErrorHandlingFunction();
$obj->doSetErrorHandlingStatic();
$obj->doSetErrorHandlingObject();
$obj->doPushErrorHandlingFunction();
$obj->doPushErrorHandlingStatic();
$obj->doPushErrorHandlingObject();
$obj->doPopErrorHandling();
$obj->doPopErrorHandling();
$obj->doPopErrorHandling();
print "*******************************************\n";
print "testing non-static error-handling in-class code\n";
print "*******************************************\n";
PEAR::setErrorHandling(PEAR_ERROR_RETURN, E_USER_NOTICE);
$obj = new testErrorHandlingPEAR;
$obj->doSetErrorHandlingFunction();
$obj->doSetErrorHandlingStatic();
$obj->doSetErrorHandlingObject();
$obj->doPushErrorHandlingFunction();
$obj->doPushErrorHandlingStatic();
$obj->doPushErrorHandlingObject();
$obj->doPopErrorHandling();
$obj->doPopErrorHandling();
$obj->doPopErrorHandling();

?>
--GET--
--POST--
--EXPECT--
testing static error-handling global code
echoing PEAR error-handling Variables:
--------------------------------------
_PEAR_default_error_mode:
PEAR_ERROR_RETURN,
_PEAR_default_error_options:
E_USER_NOTICE
--------------------------------------
testing setErrorHandling
echoing PEAR error-handling Variables:
--------------------------------------
_PEAR_default_error_mode:
PEAR_ERROR_CALLBACK,
_PEAR_default_error_options:
Callback: get_error_mode()
--------------------------------------
testing setErrorHandling array(obj, method)
echoing PEAR error-handling Variables:
--------------------------------------
_PEAR_default_error_mode:
PEAR_ERROR_CALLBACK,
_PEAR_default_error_options:
Callback: testerrorhandlingpear->fakeHandleError()
--------------------------------------
testing setErrorHandling array(class, method)
echoing PEAR error-handling Variables:
--------------------------------------
_PEAR_default_error_mode:
PEAR_ERROR_CALLBACK,
_PEAR_default_error_options:
Callback: (static) testErrorHandlingStatic::fakeHandleError()
--------------------------------------
testing pushErrorHandling
stack _PEAR_error_handler_stack:
Index 0:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: (static) testErrorHandlingStatic::fakeHandleError()
    --------------------------------------
Index 1:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: get_error_mode()
    --------------------------------------
testing pushErrorHandling array(obj, method)
stack _PEAR_error_handler_stack:
Index 0:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: (static) testErrorHandlingStatic::fakeHandleError()
    --------------------------------------
Index 1:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: get_error_mode()
    --------------------------------------
Index 2:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: get_error_mode()
    --------------------------------------
Index 3:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: testerrorhandlingpear->fakeHandleError()
    --------------------------------------
testing pushErrorHandling array(class, method)
stack _PEAR_error_handler_stack:
Index 0:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: (static) testErrorHandlingStatic::fakeHandleError()
    --------------------------------------
Index 1:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: get_error_mode()
    --------------------------------------
Index 2:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: get_error_mode()
    --------------------------------------
Index 3:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: testerrorhandlingpear->fakeHandleError()
    --------------------------------------
Index 4:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: testerrorhandlingpear->fakeHandleError()
    --------------------------------------
Index 5:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: (static) testErrorHandlingStatic::fakeHandleError()
    --------------------------------------
testing popErrorHandling
stack _PEAR_error_handler_stack:
Index 0:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: (static) testErrorHandlingStatic::fakeHandleError()
    --------------------------------------
Index 1:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: get_error_mode()
    --------------------------------------
Index 2:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: get_error_mode()
    --------------------------------------
Index 3:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: testerrorhandlingpear->fakeHandleError()
    --------------------------------------
testing popErrorHandling
stack _PEAR_error_handler_stack:
Index 0:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: (static) testErrorHandlingStatic::fakeHandleError()
    --------------------------------------
Index 1:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: get_error_mode()
    --------------------------------------
testing popErrorHandling
stack _PEAR_error_handler_stack:
*******************************************
testing static error-handling in-class code
*******************************************
testing in class setErrorHandling
echoing PEAR error-handling Variables:
--------------------------------------
_PEAR_default_error_mode:
PEAR_ERROR_CALLBACK,
_PEAR_default_error_options:
Callback: get_error_mode()
--------------------------------------
testing in class setErrorHandling array(obj, method)
echoing PEAR error-handling Variables:
--------------------------------------
_PEAR_default_error_mode:
PEAR_ERROR_CALLBACK,
_PEAR_default_error_options:
Callback: testerrorhandlingpear->fakeHandleError()
--------------------------------------
testing in class setErrorHandling array(class, method)
echoing PEAR error-handling Variables:
--------------------------------------
_PEAR_default_error_mode:
PEAR_ERROR_CALLBACK,
_PEAR_default_error_options:
Callback: (static) testErrorHandlingStatic::fakeHandleError()
--------------------------------------
testing in class pushErrorHandling
stack _PEAR_error_handler_stack:
Index 0:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: (static) testErrorHandlingStatic::fakeHandleError()
    --------------------------------------
Index 1:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: get_error_mode()
    --------------------------------------
testing in class pushErrorHandling array(class, method)
stack _PEAR_error_handler_stack:
Index 0:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: (static) testErrorHandlingStatic::fakeHandleError()
    --------------------------------------
Index 1:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: get_error_mode()
    --------------------------------------
Index 2:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: get_error_mode()
    --------------------------------------
Index 3:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: (static) testErrorHandlingStatic::fakeHandleError()
    --------------------------------------
testing in class pushErrorHandling array(obj, method)
stack _PEAR_error_handler_stack:
Index 0:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: (static) testErrorHandlingStatic::fakeHandleError()
    --------------------------------------
Index 1:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: get_error_mode()
    --------------------------------------
Index 2:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: get_error_mode()
    --------------------------------------
Index 3:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: (static) testErrorHandlingStatic::fakeHandleError()
    --------------------------------------
Index 4:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: (static) testErrorHandlingStatic::fakeHandleError()
    --------------------------------------
Index 5:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: testerrorhandlingpear->fakeHandleError()
    --------------------------------------
testing in class popErrorHandling
stack _PEAR_error_handler_stack:
Index 0:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: (static) testErrorHandlingStatic::fakeHandleError()
    --------------------------------------
Index 1:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: get_error_mode()
    --------------------------------------
Index 2:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: get_error_mode()
    --------------------------------------
Index 3:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: (static) testErrorHandlingStatic::fakeHandleError()
    --------------------------------------
testing in class popErrorHandling
stack _PEAR_error_handler_stack:
Index 0:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: (static) testErrorHandlingStatic::fakeHandleError()
    --------------------------------------
Index 1:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: get_error_mode()
    --------------------------------------
testing in class popErrorHandling
stack _PEAR_error_handler_stack:
*******************************************
testing non-static error-handling in-class code
*******************************************
testing in PEAR setErrorHandling
echoing PEAR error-handling Variables:
--------------------------------------
_PEAR_default_error_mode:
PEAR_ERROR_RETURN,
_PEAR_default_error_options:
E_USER_NOTICE
--------------------------------------
echoing PEAR error-handling Variables:
--------------------------------------
$this->_default_error_mode:
PEAR_ERROR_CALLBACK,
$this->_default_error_options:
Callback: get_error_mode()
--------------------------------------
testing in PEAR setErrorHandling array(obj, method)
echoing PEAR error-handling Variables:
--------------------------------------
_PEAR_default_error_mode:
PEAR_ERROR_RETURN,
_PEAR_default_error_options:
E_USER_NOTICE
--------------------------------------
echoing PEAR error-handling Variables:
--------------------------------------
$this->_default_error_mode:
PEAR_ERROR_CALLBACK,
$this->_default_error_options:
Callback: testerrorhandlingpear->fakeHandleError()
--------------------------------------
testing in PEAR setErrorHandling array(class, method)
echoing PEAR error-handling Variables:
--------------------------------------
_PEAR_default_error_mode:
PEAR_ERROR_RETURN,
_PEAR_default_error_options:
E_USER_NOTICE
--------------------------------------
echoing PEAR error-handling Variables:
--------------------------------------
$this->_default_error_mode:
PEAR_ERROR_CALLBACK,
$this->_default_error_options:
Callback: (static) testErrorHandlingStatic::fakeHandleError()
--------------------------------------
testing in PEAR pushErrorHandling
stack _PEAR_error_handler_stack:
Index 0:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: (static) testErrorHandlingStatic::fakeHandleError()
    --------------------------------------
Index 1:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: get_error_mode()
    --------------------------------------
testing in PEAR pushErrorHandling array(class, method)
stack _PEAR_error_handler_stack:
Index 0:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: (static) testErrorHandlingStatic::fakeHandleError()
    --------------------------------------
Index 1:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: get_error_mode()
    --------------------------------------
Index 2:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: get_error_mode()
    --------------------------------------
Index 3:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: (static) testErrorHandlingStatic::fakeHandleError()
    --------------------------------------
testing in PEAR pushErrorHandling array(obj, method)
stack _PEAR_error_handler_stack:
Index 0:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: (static) testErrorHandlingStatic::fakeHandleError()
    --------------------------------------
Index 1:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: get_error_mode()
    --------------------------------------
Index 2:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: get_error_mode()
    --------------------------------------
Index 3:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: (static) testErrorHandlingStatic::fakeHandleError()
    --------------------------------------
Index 4:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: (static) testErrorHandlingStatic::fakeHandleError()
    --------------------------------------
Index 5:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: testerrorhandlingpear->fakeHandleError()
    --------------------------------------
testing in PEAR popErrorHandling
stack _PEAR_error_handler_stack:
Index 0:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: (static) testErrorHandlingStatic::fakeHandleError()
    --------------------------------------
Index 1:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: get_error_mode()
    --------------------------------------
Index 2:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: get_error_mode()
    --------------------------------------
Index 3:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: (static) testErrorHandlingStatic::fakeHandleError()
    --------------------------------------
testing in PEAR popErrorHandling
stack _PEAR_error_handler_stack:
Index 0:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: (static) testErrorHandlingStatic::fakeHandleError()
    --------------------------------------
Index 1:
    echoing PEAR error-handling Variables:
    --------------------------------------
    mode:
    PEAR_ERROR_CALLBACK,
    options:
    Callback: get_error_mode()
    --------------------------------------
testing in PEAR popErrorHandling
stack _PEAR_error_handler_stack:
