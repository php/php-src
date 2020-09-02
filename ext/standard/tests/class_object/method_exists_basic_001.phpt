--TEST--
method_exists() on userspace classes; static & non-static methods with various visibilities.
--FILE--
<?php
Class B {
    public function inherit_pub() {}
    protected function inherit_prot() {}
    private function inherit_priv() {}
    static public function inherit_static_pub() {}
    static protected function inherit_static_prot() {}
    static private function inherit_static_priv() {}
}

Class C extends B {
    public function pub() {}
    protected function prot() {}
    private function priv() {}
    static public function static_pub() {}
    static protected function static_prot() {}
    static private function static_priv() {}
}


$methods = array(
    'inherit_pub', 'inherit_prot', 'inherit_priv',
    'inherit_static_pub', 'inherit_static_prot', 'inherit_static_priv',
    'pub', 'prot', 'priv',
    'static_pub', 'static_prot', 'static_priv',
    'non_existent');

echo "\n ---(Using string class name)---\n";
foreach ($methods as $method) {
  echo "Does C::$method exist? ";
  var_dump(method_exists("C", $method));
}

echo "\n ---(Using object)---\n";
$myC = new C;
foreach ($methods as $method) {
  echo "Does C::$method exist? ";
  var_dump(method_exists($myC, $method));
}

echo "Done";
?>
--EXPECT--
---(Using string class name)---
Does C::inherit_pub exist? bool(true)
Does C::inherit_prot exist? bool(true)
Does C::inherit_priv exist? bool(false)
Does C::inherit_static_pub exist? bool(true)
Does C::inherit_static_prot exist? bool(true)
Does C::inherit_static_priv exist? bool(false)
Does C::pub exist? bool(true)
Does C::prot exist? bool(true)
Does C::priv exist? bool(true)
Does C::static_pub exist? bool(true)
Does C::static_prot exist? bool(true)
Does C::static_priv exist? bool(true)
Does C::non_existent exist? bool(false)

 ---(Using object)---
Does C::inherit_pub exist? bool(true)
Does C::inherit_prot exist? bool(true)
Does C::inherit_priv exist? bool(true)
Does C::inherit_static_pub exist? bool(true)
Does C::inherit_static_prot exist? bool(true)
Does C::inherit_static_priv exist? bool(true)
Does C::pub exist? bool(true)
Does C::prot exist? bool(true)
Does C::priv exist? bool(true)
Does C::static_pub exist? bool(true)
Does C::static_prot exist? bool(true)
Does C::static_priv exist? bool(true)
Does C::non_existent exist? bool(false)
Done
