<?php
	function inspectClass($class) {

		/* not used: public ReflectionClass[] getInterfaces()  */

		printf("\nInspecting class '%s'\n", $class->getName());
		printf("isInternal: %s\n", ($class->isInternal()) ? 'yes' : 'no');
		printf("isUserDefined: %s\n", ($class->isUserDefined()) ? 'yes' : 'no');
		printf("isInstantiable: %s\n", ($class->isInstantiable()) ? 'yes' : 'no');
		printf("isInterface: %s\n", ($class->isInterface()) ? 'yes' : 'no');
		printf("isAbstract: %s\n", ($class->isAbstract()) ? 'yes' : 'no');
		printf("isFinal: %s\n", ($class->isFinal()) ? 'yes' : 'no');
		printf("isIteratable: %s\n", ($class->isIterateable()) ? 'yes' : 'no');
		printf("Modifiers: '%d'\n", $class->getModifiers());
		printf("Parent Class: '%s'\n", $class->getParentClass());
		printf("Extension: '%s'\n", $class->getExtensionName());

		if ($method = $class->getConstructor())
			inspectMethod($method);

		if ($methods = $class->getMethods()) {
			$tmp = array();
			foreach ($methods as $method)
				$tmp[$method->getName()] = $method;

			ksort($tmp, SORT_STRING);
			foreach ($tmp as $method)
				inspectMethod($method);
		}

		if ($properties = $class->getProperties()) {
			$tmp = array();
			foreach ($properties as $prop)
				$tmp[$prop->getName()] = $prop;
			ksort($tmp, SORT_STRING);
			foreach ($tmp as $prop)
			inspectProperty($prop);
		}


		if ($properties = $class->getDefaultProperties()) {
			ksort($properties, SORT_STRING);
			foreach ($properties as $name => $v)
				printf("Default property '%s'\n", $name);
		}

		if ($properties = $class->getStaticProperties()) {
			ksort($properties, SORT_STRING);
			foreach ($properties as $name => $v)
				printf("Static property '%s'\n", $name);
		}

		if ($constants = $class->getConstants()) {
			ksort($constants, SORT_STRING);
			foreach ($constant as $name => $value)
				printf("Constant '%s' = '%s'\n", $name, $value);
		}

	}

	function inspectProperty(&$prop) {

		printf("\nInspecting property '%s'\n", $prop->getName());
		printf("isPublic: %s\n", ($prop->isPublic()) ? 'yes' : 'no');
		printf("isPrivate: %s\n", ($prop->isPrivate()) ? 'yes' : 'no');
		printf("isProtected: %s\n", ($prop->isProtected()) ? 'yes' : 'no');
		printf("isStatic: %s\n", ($prop->isStatic()) ? 'yes' : 'no');
		printf("isDefault: %s\n", ($prop->isDefault()) ? 'yes' : 'no');
		printf("Modifiers: %d\n", $prop->getModifiers());
		// printf("Value\n"); var_export($prop->getValue());

	}

	function inspectMethod(&$method) {

		printf("\nInspecting method '%s'\n", $method->getName());
		printf("isFinal: %s\n", ($method->isFinal()) ? 'yes' : 'no');
		printf("isAbstract: %s\n", ($method->isAbstract()) ? 'yes' : 'no');
		printf("isPublic: %s\n", ($method->isPublic()) ? 'yes' : 'no');
		printf("isPrivate: %s\n", ($method->isPrivate()) ? 'yes' : 'no');
		printf("isProtected: %s\n", ($method->isProtected()) ? 'yes' : 'no');
		printf("isStatic: %s\n", ($method->isStatic()) ? 'yes' : 'no');
		printf("isConstructor: %s\n", ($method->isConstructor()) ? 'yes' : 'no');
		printf("isDestructor: %s\n", ($method->isDestructor()) ? 'yes' : 'no');
		printf("isInternal: %s\n", ($method->isInternal()) ? 'yes' : 'no');
		printf("isUserDefined: %s\n", ($method->isUserDefined()) ? 'yes' : 'no');
		printf("returnsReference: %s\n", ($method->returnsReference()) ? 'yes' : 'no');
		printf("Modifiers: %d\n", $method->getModifiers());
		printf("Number of Parameters: %d\n", $method->getNumberOfParameters());
		printf("Number of Required Parameters: %d\n", $method->getNumberOfRequiredParameters());

		if ($params = $method->getParameters()) {
			$tmp = array();
			foreach ($params as $k => $param)
				$tmp[$param->getName()] = $param;

//			ksort($tmp, SORT_STRING);
			foreach ($tmp as $param)
				inspectParameter($method, $param);
		}

		if ($static = $method->getStaticVariables()) {
			sort($static, SORT_STRING);
			printf("Static variables: %s\n", implode('/', $static));
		}

	}

	function inspectParameter(&$method, &$param) {

		printf("\nInspecting parameter '%s' of method '%s'\n",
		$param->getName(), $method->getName());
		printf("isArray: %s\n", ($param->isArray()) ? 'yes': 'no');
		printf("allowsNull: %s\n", ($param->allowsNull()) ? 'yes' : 'no');
		printf("isPassedByReference: %s\n", ($param->isPassedByReference()) ? 'yes' : 'no');
		printf("isOptional: %s\n", ($param->isOptional()) ? 'yes' : 'no');
		printf("isDefaultValueAvailable: %s\n", ($param->isDefaultValueAvailable()) ? 'yes' : 'no');
		// printf("getDefaultValue: %s\n", ($param->getDefaultValue()) ? 'yes' : 'no');

	}
?>
