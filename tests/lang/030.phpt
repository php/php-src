--TEST--
$this in constructor test
--FILE--
<?php
class foo {
    public $Name;
    function __construct($name) {
        $GLOBALS['List']= &$this;
        $this->Name = $name;
        $GLOBALS['List']->echoName();
    }

    function echoName() {
        $GLOBALS['names'][]=$this->Name;
    }
}

function &foo2(&$foo) {
    return $foo;
}


$bar1 =new foo('constructor');
$bar1->Name = 'outside';
$bar1->echoName();
$List->echoName();

$foo = new foo('constructor');
$bar1 =& foo2($foo);
$bar1->Name = 'outside';
$bar1->echoName();

$List->echoName();

print ($names==array('constructor','outside','outside','constructor','outside','outside')) ? 'success':'failure';
?>
--EXPECT--
success
