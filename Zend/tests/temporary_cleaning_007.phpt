--TEST--
Exception inside a foreach loop with return
--FILE--
<?php
class saboteurTestController {
    public function isConsistent() { throw new \Exception(); }
}

$controllers = array(new saboteurTestController(),new saboteurTestController());
foreach ($controllers as $controller) {	
    try {
        if ($controller->isConsistent()) {
            return $controller;
        }
    } catch (\Exception $e) {
        echo "Exception\n";
    }
}
?>
--EXPECT--
Exception
Exception
