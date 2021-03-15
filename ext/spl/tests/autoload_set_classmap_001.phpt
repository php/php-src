--TEST--
autoload_set_classmap Basic Operation
--FILE--
<?php

$classes = ['Example', 'Foo\NamespacedExample'];
$classes_map = [];

foreach ($classes as $class) {
    $p = explode("\\", $class);
    $class_name = array_pop($p);
    $namespace = implode('\\', $p);

    $class = strtolower($class);

    $class_str = '<?php ' . ($namespace ? ('namespace ' . $namespace . ';') : '') . ' class ' . $class_name . '{ }';
    $path = sys_get_temp_dir() . '/' . $class . '.php';
    file_put_contents($path, $class_str);

    $classes_map[$class] = $path;
}

autoload_set_classmap($classes_map);

echo get_debug_type(new Example()) . "\n";
echo get_debug_type(new Foo\NamespacedExample()) . "\n";

?>
--EXPECT--
Example
Foo\NamespacedExample