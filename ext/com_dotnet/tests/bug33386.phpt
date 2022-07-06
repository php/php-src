--TEST--
Bug #33386 (ScriptControl only sees last function of class)
--SKIPIF--
<?php
if (!extension_loaded("com_dotnet")) print "skip COM/.Net support not present";
if (4 != PHP_INT_SIZE)  print "skip MSScriptControl isn't available under x64";
?>
--FILE--
<?php
error_reporting(E_ALL);

class twoFuncs {
    public function func1() { echo " func one\n"; }
    public function func2() { echo " func two\n"; }
}

try {
    $ciTF = new twoFuncs;

    $oScript = new COM("MSScriptControl.ScriptControl");
    $oScript->Language = "VBScript";

    $oScript->AddObject ("tfA", $ciTF, true);
    foreach (array(1,2) as $i) {
        $oScript->ExecuteStatement ("tfA.func$i");
        $oScript->ExecuteStatement ("func$i");
    }
    $oScript->AddObject ("tfB", $ciTF);
    foreach (array(1,2) as $i) {
        $oScript->ExecuteStatement ("tfB.func$i");
    }
} catch (Exception $e) {
    print $e;
}
?>
--EXPECT--
 func one
 func one
 func two
 func two
 func one
 func two
