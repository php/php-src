--TEST--
ReflectionAttribute::getCurrent() specific return type
--FILE--
<?php

#[Attribute]
class Demo {
	public function __construct($args) {
		echo get_class(ReflectionAttribute::getCurrent()) . " (on $args)\n";
	}
}

#[Demo("class")]
class DemoClass {
	#[Demo("class constant")]
	public const FOO = 'BAR';

	#[Demo("class property")]
	public mixed $prop {
		set(
			#[Demo("class property hook parameter")]
			mixed $value
		) {}
	}

	#[Demo("class method")]
	public function method(
		#[Demo("class method parameter")] $param
	) {}
}

#[Demo("trait")]
trait DemoTrait {
	#[Demo("trait constant")]
	public const FOO = 'BAR';

	#[Demo("trait property")]
	public mixed $prop;

	#[Demo("trait method")]
	public function method(
		#[Demo("trait method parameter")] $param
	) {}
}

#[Demo("interface")]
interface DemoInterface {
	#[Demo("interface constant")]
	public const FOO = 'BAR';

	#[Demo("interface property")]
	public mixed $prop { get; set; }

	#[Demo("interface method")]
	public function method(
		#[Demo("trait method parameter")] $param
	);
}

#[Demo("unit enum")]
enum MyUnitEnum {
	#[Demo("unit enum case")]
	case ExampleCase;

	#[Demo("unit enum constant")]
	public const FOO = 'BAR';

	#[Demo("unit enum method")]
	public function method(
		#[Demo("unit enum method parameter")] $param
	) {}
}

#[Demo("backed enum")]
enum MyBackedEnum: string {
	#[Demo("backed enum case")]
	case ExampleCase = 'Example';

	#[Demo("backed enum constant")]
	public const FOO = 'BAR';

	#[Demo("backed enum method")]
	public function method(
		#[Demo("backed enum method parameter")] $param
	) {}
}

#[Demo("global function")]
function globalFunc(
	#[Demo("global function parameter")] $param
) {}

$closure = #[Demo("closure")] static function (#[Demo("closure param")] $param) {};

#[Demo("global constant")]
const GLOBAL_CONSTANT = true;

$cases = [
	'For a class' => [
		new ReflectionClass(DemoClass::class),
		new ReflectionObject(new DemoClass()),
		new ReflectionClassConstant(DemoClass::class, 'FOO'),
		new ReflectionMethod(DemoClass::class, 'method'),
		new ReflectionParameter([DemoClass::class, 'method'], 0),
		new ReflectionProperty(DemoClass::class, 'prop'),
		new ReflectionProperty(DemoClass::class, 'prop')
			->getHook(PropertyHookType::Set)
			->getParameters()[0],
	],

	'For a trait' => [
		new ReflectionClass(DemoTrait::class),
		new ReflectionClassConstant(DemoTrait::class, 'FOO'),
		new ReflectionMethod(DemoTrait::class, 'method'),
		new ReflectionParameter([DemoTrait::class, 'method'], 0),
	],

	'For an interface' => [
		new ReflectionClass(DemoInterface::class),
		new ReflectionClassConstant(DemoInterface::class, 'FOO'),
		new ReflectionMethod(DemoTrait::class, 'method'),
		new ReflectionParameter([DemoTrait::class, 'method'], 0),
	],

	'For a unit enum' => [
		new ReflectionClass(MyUnitEnum::class),
		new ReflectionEnum(MyUnitEnum::class),
		new ReflectionObject(MyUnitEnum::ExampleCase),
		new ReflectionEnumUnitCase(MyUnitEnum::class, 'ExampleCase'),
		new ReflectionClassConstant(MyUnitEnum::class, 'FOO'),
		new ReflectionMethod(MyUnitEnum::class, 'method'),
		new ReflectionParameter([MyUnitEnum::class, 'method'], 0),
	],

	'For a backed enum' => [
		new ReflectionClass(MyBackedEnum::class),
		new ReflectionEnum(MyBackedEnum::class),
		new ReflectionObject(MyBackedEnum::ExampleCase),
		new ReflectionEnumUnitCase(MyBackedEnum::class, 'ExampleCase'),
		new ReflectionEnumBackedCase(MyBackedEnum::class, 'ExampleCase'),
		new ReflectionClassConstant(MyBackedEnum::class, 'FOO'),
		new ReflectionMethod(MyBackedEnum::class, 'method'),
		new ReflectionParameter([MyBackedEnum::class, 'method'], 0),
	],

	'For a global function' => [
		new ReflectionFunction('globalFunc'),
		new ReflectionParameter('globalFunc', 0),
	],

	'For a closure' => [
		new ReflectionFunction($closure),
		new ReflectionParameter($closure, 0),
	],

	'For a global constant' => [
		new ReflectionConstant('GLOBAL_CONSTANT'),
	],
];

$first = true;
foreach ($cases as $key => $data) {
	if ($first) {
		$first = false;
	} else {
		echo "\n\n";
	}
	echo "$key:\n";
	foreach ($data as $label => $case) {
		echo get_class($case) . " -> ";
		$case->getAttributes()[0]->newInstance();
	}
}
?>
--EXPECT--
For a class:
ReflectionClass -> ReflectionClass (on class)
ReflectionObject -> ReflectionClass (on class)
ReflectionClassConstant -> ReflectionClassConstant (on class constant)
ReflectionMethod -> ReflectionMethod (on class method)
ReflectionParameter -> ReflectionParameter (on class method parameter)
ReflectionProperty -> ReflectionProperty (on class property)
ReflectionParameter -> ReflectionParameter (on class property hook parameter)


For a trait:
ReflectionClass -> ReflectionClass (on trait)
ReflectionClassConstant -> ReflectionClassConstant (on trait constant)
ReflectionMethod -> ReflectionMethod (on trait method)
ReflectionParameter -> ReflectionParameter (on trait method parameter)


For an interface:
ReflectionClass -> ReflectionClass (on interface)
ReflectionClassConstant -> ReflectionClassConstant (on interface constant)
ReflectionMethod -> ReflectionMethod (on trait method)
ReflectionParameter -> ReflectionParameter (on trait method parameter)


For a unit enum:
ReflectionClass -> ReflectionEnum (on unit enum)
ReflectionEnum -> ReflectionEnum (on unit enum)
ReflectionObject -> ReflectionEnum (on unit enum)
ReflectionEnumUnitCase -> ReflectionClassConstant (on unit enum case)
ReflectionClassConstant -> ReflectionClassConstant (on unit enum constant)
ReflectionMethod -> ReflectionMethod (on unit enum method)
ReflectionParameter -> ReflectionParameter (on unit enum method parameter)


For a backed enum:
ReflectionClass -> ReflectionEnum (on backed enum)
ReflectionEnum -> ReflectionEnum (on backed enum)
ReflectionObject -> ReflectionEnum (on backed enum)
ReflectionEnumUnitCase -> ReflectionClassConstant (on backed enum case)
ReflectionEnumBackedCase -> ReflectionClassConstant (on backed enum case)
ReflectionClassConstant -> ReflectionClassConstant (on backed enum constant)
ReflectionMethod -> ReflectionMethod (on backed enum method)
ReflectionParameter -> ReflectionParameter (on backed enum method parameter)


For a global function:
ReflectionFunction -> ReflectionFunction (on global function)
ReflectionParameter -> ReflectionParameter (on global function parameter)


For a closure:
ReflectionFunction -> ReflectionFunction (on closure)
ReflectionParameter -> ReflectionParameter (on closure param)


For a global constant:
ReflectionConstant -> ReflectionConstant (on global constant)
